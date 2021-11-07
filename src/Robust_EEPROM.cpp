#include "Robust_EEPROM.h"

uint16_t mathFunctions::max_uint (uint16_t first, uint16_t second) {
    if (first < second) {
        return second;
    } else {
        return first;
    }
    
}

uint16_t mathFunctions::min_uint (uint16_t first, uint16_t second) {
    if (first < second) {
        return first;
    } else {
        return second;
    }
    
}

uint16_t mathFunctions::ceil_log2 (uint16_t first) {

    uint16_t log2;
    for (log2 = 1; 2^log2 < first; log2++) {}

    return log2;
}

Dummy_EEPROM::Dummy_EEPROM (uint16_t size) {

    dummy_bytes = new uint8_t[size];
    ttl_bytes = new uint16_t[size];
    this->size = size;

    for (uint16_t b = 0; b < size; b++) {
        dummy_bytes[b] = (uint8_t)(rand() % 256);
        ttl_bytes[b] = (uint16_t)(65535 - rand() % 30000);    // maximum for uint16_t = 65535
    }

}

uint16_t Dummy_EEPROM::length () {
    return size;
}

uint8_t Dummy_EEPROM::read (uint16_t read_byte) {
    return dummy_bytes[read_byte];
}

void Dummy_EEPROM::write (uint16_t write_byte, uint8_t data) {

    if (ttl_bytes[write_byte] > 0) {
        dummy_bytes[write_byte] = data;
        ttl_bytes[write_byte]--;
    }

}

void Dummy_EEPROM::update (uint16_t update_byte, uint8_t data) {
    if (read(update_byte) != data)
        write(update_byte, data);
}

Robust_EEPROM::Robust_EEPROM (uint16_t firstByte, uint16_t lengthBytes, Dummy_EEPROM* const dummy_eeprom) {

    Serial.println("FINAL");

    this->dummy_eeprom = dummy_eeprom;

    if (dummy_eeprom == nullptr) {

        totalBytes = mathFunctions::min_uint(lengthBytes, EEPROM.length());
        this->firstByte = mathFunctions::min_uint(firstByte, EEPROM.length() - lengthBytes);

    } else {

        Serial.println(dummy_eeprom->length());

        totalBytes = mathFunctions::min_uint(lengthBytes, dummy_eeprom->length());
        this->firstByte = mathFunctions::min_uint(firstByte, dummy_eeprom->length() - lengthBytes);

    }

    lastDataByte = this->firstByte;

    totalControlBytes = (uint16_t)ceil((double)totalBytes/9) + 2;
    totalDataBytes = totalBytes - totalControlBytes;

}

Robust_EEPROM::Robust_EEPROM (Dummy_EEPROM* const dummy_eeprom) {

    Serial.println("dummy_eeprom");

    this->dummy_eeprom = dummy_eeprom;

    totalBytes = dummy_eeprom->length();

    firstByte = 0;
    lastDataByte = firstByte;

    totalControlBytes = (uint16_t)ceil((double)totalBytes/9) + 2;
    totalDataBytes = totalBytes - totalControlBytes;
    
}

Robust_EEPROM::Robust_EEPROM () {

    Serial.println("EEPROM");

    dummy_eeprom = nullptr;

    totalBytes = EEPROM.length();
    firstByte = 0;
    lastDataByte = firstByte;

    totalControlBytes = (uint16_t)ceil((double)totalBytes/9) + 2;
    totalDataBytes = totalBytes - totalControlBytes;

}

uint16_t Robust_EEPROM::length () {

    uint16_t failed_bytes = 0;
    uint8_t temp_byte = 0;

    if (dummy_eeprom == nullptr) {
        for (uint16_t i = firstByte + absolutelength() - 1; i >= firstByte + datalength() + 2 - 1; i--) {
            temp_byte = EEPROM.read(i);
            for (int b = 0; b < 8; b++) {
                failed_bytes += temp_byte >> b & 0b00000001;
            }
        }
    } else {
        for (uint16_t i = firstByte + absolutelength() - 1; i >= firstByte + datalength() + 2 - 1; i--) {
            temp_byte = dummy_eeprom->read(i);
            for (int b = 0; b < 8; b++) {
                failed_bytes += temp_byte >> b & 0b00000001;
            }
        }
    }

    return totalDataBytes - failed_bytes;

}

uint16_t Robust_EEPROM::absolutelength () {

    return totalDataBytes + totalControlBytes;

}

uint16_t Robust_EEPROM::datalength () {

    return totalDataBytes;

}

uint16_t Robust_EEPROM::controllength () {

    return totalControlBytes;

}

uint16_t Robust_EEPROM::absolutebyte (uint16_t relative_byte) {

    int16_t absolute_byte = mathFunctions::max_uint(relative_byte, firstByte);
    uint8_t temp_byte = 0;

    if (dummy_eeprom == nullptr) {
        for (uint16_t i = firstByte + absolutelength() - 1; i >= firstByte + absolutelength() - 1 - absolute_byte; i--) {
            if (i < firstByte + datalength() + 2 - 1)
                return lastDataByte;
            temp_byte = EEPROM.read(i);
            for (int b = 0; b < 8; b++) {
                absolute_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
            }
        }
        return absolute_byte;
    } else {
        for (uint16_t i = firstByte + absolutelength() - 1; i >= firstByte + absolutelength() - 1 - absolute_byte; i--) {
            if (i < firstByte + datalength() + 2 - 1)
                return lastDataByte;
            temp_byte = dummy_eeprom->read(i);
            for (int b = 0; b < 8; b++) {
                absolute_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
            }
        }
        return absolute_byte;
    }

}

uint8_t Robust_EEPROM::read (uint16_t read_byte) {

    lastDataByte = mathFunctions::min_uint(read_byte, firstByte + totalDataBytes - 1);
    if (dummy_eeprom == nullptr) {
        return EEPROM.read(absolutebyte(read_byte));
    } else {
        return dummy_eeprom->read(absolutebyte(read_byte));
    }

}

void Robust_EEPROM::write (uint16_t write_byte, uint8_t data) {

    lastDataByte = mathFunctions::min_uint(write_byte, firstByte + totalDataBytes - 1);
    if (dummy_eeprom == nullptr) {
        EEPROM.write(absolutebyte(write_byte), data);
    } else {
        dummy_eeprom->write(absolutebyte(write_byte), data);
    }
    lastDataByte = mathFunctions::max_uint(write_byte, lastDataByte);

}

void Robust_EEPROM::update (uint16_t update_byte, uint8_t data) {

    lastDataByte = mathFunctions::min_uint(update_byte, firstByte + totalDataBytes - 1);
    if (dummy_eeprom == nullptr) {
        EEPROM.update(absolutebyte(update_byte), data);
    } else {
        dummy_eeprom->update(absolutebyte(update_byte), data);
    }
    lastDataByte = mathFunctions::max_uint(update_byte, lastDataByte);

}

void Robust_EEPROM::fullreset () {

    if (dummy_eeprom == nullptr) {
        for (uint16_t i = firstByte; i < firstByte + absolutelength(); i++) {
            EEPROM.write(i, 0);
        }
    } else {
        for (uint16_t i = firstByte; i < firstByte + absolutelength(); i++) {
            dummy_eeprom->write(i, 0);
        }
    }

}