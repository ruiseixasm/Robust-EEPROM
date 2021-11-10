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

uint16_t mathFunctions::seed_generator (uint32_t total_duration, uint32_t fragmental_duration) {

    uint16_t magic_number = 0;
    uint32_t start_stamp = millis();
    uint32_t actual_stamp = start_stamp;

    while (labs(millis() - start_stamp) < total_duration) {
        magic_number++;
        if (labs(millis() - actual_stamp) >= fragmental_duration) {
            if (analogRead(0) % 2 == 0) {
                magic_number /= 2;
            } else {
                magic_number *= 2;
            }
            actual_stamp = millis();
        }
    }
    return magic_number;
}

Dummy_EEPROM::Dummy_EEPROM (uint16_t size) {

    dummy_bytes = new uint8_t[size];
    ttl_bytes = new uint16_t[size];
    this->size = size;
    srand(mathFunctions::seed_generator());

    for (uint16_t b = 0; b < size; b++) {
        dummy_bytes[b] = (uint8_t)(rand() % 256);
        ttl_bytes[b] = (uint16_t)(1000 - rand() % 500);    // maximum for uint16_t = 65535
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

    this->dummy_eeprom = dummy_eeprom;

    this->firstByte = mathFunctions::min_uint(firstByte, dummy_eeprom->length() - 1);
    totalBytes = mathFunctions::min_uint(lengthBytes, dummy_eeprom->length() - this->firstByte);

    initiateEEPROM();
}

Robust_EEPROM::Robust_EEPROM (uint16_t firstByte, uint16_t lengthBytes) {

    dummy_eeprom = nullptr;

    this->firstByte = mathFunctions::min_uint(firstByte, EEPROM.length() - 1);
    totalBytes = mathFunctions::min_uint(lengthBytes, EEPROM.length() - this->firstByte);

    initiateEEPROM();

}

Robust_EEPROM::Robust_EEPROM (Dummy_EEPROM* const dummy_eeprom) {

    this->dummy_eeprom = dummy_eeprom;

    firstByte = 0;
    totalBytes = dummy_eeprom->length();

    initiateEEPROM();

}

Robust_EEPROM::Robust_EEPROM () {

    dummy_eeprom = nullptr;

    firstByte = 0;
    totalBytes = EEPROM.length();

    initiateEEPROM();
}

void Robust_EEPROM::initiateEEPROM () {
    totalControlBytes = (uint16_t)ceil((double)totalBytes/9);
    totalDataBytes = totalBytes - totalControlBytes;
}

uint16_t Robust_EEPROM::length () {

    uint16_t failed_bytes = 0;
    uint8_t temp_byte = 0;

    if (dummy_eeprom == nullptr) {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + totalDataBytes + 2 - 1; control_i--) {
            temp_byte = EEPROM.read(control_i);
            for (int b = 0; b < 8; b++) {
                failed_bytes += temp_byte >> b & 0b00000001;
            }
        }
    } else {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + totalDataBytes + 2 - 1; control_i--) {
            temp_byte = dummy_eeprom->read(control_i);
            for (int b = 0; b < 8; b++) {
                failed_bytes += temp_byte >> b & 0b00000001;
            }
        }
    }

    return totalDataBytes - failed_bytes;

}

uint16_t Robust_EEPROM::absolutelength () {
    return totalBytes;
}

uint16_t Robust_EEPROM::datalength () {
    return totalDataBytes;
}

uint16_t Robust_EEPROM::controllength () {
    return totalControlBytes;
}

uint16_t Robust_EEPROM::absolutebyte (uint16_t relative_byte) {

    uint16_t absolute_byte = 0;
    uint8_t temp_byte = 0;

    if (dummy_eeprom == nullptr) {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + totalDataBytes + 2 - 1; control_i--) {
            temp_byte = EEPROM.read(control_i);
            for (int b = 0; b < 8; b++) {
                relative_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
                if (relative_byte == 0 || absolute_byte == firstByte + totalDataBytes - 1) {
                    return absolute_byte + firstByte;
                } else {
                    relative_byte--;
                    absolute_byte++;
                }

            }
        }
    } else {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + totalDataBytes + 2 - 1; control_i--) {
            temp_byte = dummy_eeprom->read(control_i);
            for (int b = 0; b < 8; b++) {
                relative_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
                if (relative_byte == 0 || absolute_byte == firstByte + totalDataBytes - 1) {
                    return absolute_byte + firstByte;
                } else {
                    relative_byte--;
                    absolute_byte++;
                }

            }
        }
    }
    return absolute_byte + firstByte;

}

uint8_t Robust_EEPROM::read (uint16_t read_byte) {

    read_byte = mathFunctions::min_uint(read_byte, totalDataBytes - 1);
    if (dummy_eeprom == nullptr) {
        return EEPROM.read(absolutebyte(read_byte));
    } else {
        return dummy_eeprom->read(absolutebyte(read_byte));
    }

}

void Robust_EEPROM::write (uint16_t write_byte, uint8_t data) {

    write_byte = mathFunctions::min_uint(write_byte, totalDataBytes - 1);
    int tryouts = 0;
    do {
        if (dummy_eeprom == nullptr) {
            EEPROM.write(absolutebyte(write_byte), data);
        } else {
            dummy_eeprom->write(absolutebyte(write_byte), data);
        }
        if (tryouts == 5) {
            offsetright(write_byte);
            disablebyte(write_byte);
            tryouts = 0;
        }
        tryouts++;
    } while (read(write_byte) != data);

}

void Robust_EEPROM::update (uint16_t update_byte, uint8_t data) {

    update_byte = mathFunctions::min_uint(update_byte, totalDataBytes - 1);
    int tryouts = 0;
    while (read(update_byte) != data) {
        if (dummy_eeprom == nullptr) {
            EEPROM.update(absolutebyte(update_byte), data);
        } else {
            dummy_eeprom->update(absolutebyte(update_byte), data);
        }
        if (tryouts == 5) {
            offsetright(update_byte);
            disablebyte(update_byte);
            tryouts = 0;
        }
        tryouts++;
    }

}

void Robust_EEPROM::offsetright (uint16_t actual_byte) {

    int tryouts;
    for (uint16_t data_i = totalDataBytes - 1; data_i > actual_byte + 1; data_i--) {  // the actual_byte is destined to be overwritten (no need to offset)
        tryouts = 0; // needs to be reseted for each for
        while (read(data_i) != read(data_i - 1)) {

            // if (true) {
            
            //     Serial.print(actual_byte);
            //     Serial.print(":");
            //     Serial.print(absolutebyte(actual_byte));
            //     Serial.print(" - ");
            //     Serial.print(data_i - 1);
            //     Serial.print(":");
            //     Serial.print(data_i);
            //     Serial.print(" ~ ");
            //     Serial.print(absolutebyte(data_i - 1));
            //     Serial.print(":");
            //     Serial.print(absolutebyte(data_i));
            //     Serial.print(" = ");
            //     Serial.print(read(data_i - 1));
            //     Serial.print(":");
            //     Serial.println(read(data_i));
            
            // }
            
            if (dummy_eeprom == nullptr) {
                EEPROM.update(absolutebyte(data_i), read(data_i - 1));
            } else {
                dummy_eeprom->update(absolutebyte(data_i), read(data_i - 1));
            }
            if (tryouts == 5) {
                offsetright(data_i);
                disablebyte(data_i);
                tryouts = 0;
            }
            tryouts++;
            // delay(1000);
        }
    }
}

void Robust_EEPROM::disablebyte (uint16_t relative_byte) {

    uint8_t temp_byte = 0;
    
    if (dummy_eeprom == nullptr) {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + totalDataBytes + 2 - 1; control_i--) {
            temp_byte = EEPROM.read(control_i);
            for (int b = 0; b < 8; b++) {
                relative_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
                if (relative_byte == 0) {
                    temp_byte |= 0b00000001 << b;
                    EEPROM.write(control_i, temp_byte);
                    return;
                } else {
                    relative_byte--;
                }
            }
        }
    } else {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + totalDataBytes + 2 - 1; control_i--) {
            temp_byte = dummy_eeprom->read(control_i);
            for (int b = 0; b < 8; b++) {
                relative_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
                if (relative_byte == 0) {
                    temp_byte |= 0b00000001 << b;
                    dummy_eeprom->write(control_i, temp_byte);
                    return;
                } else {
                    relative_byte--;
                }
            }
        }
    }

}

void Robust_EEPROM::fullreset () {
    if (dummy_eeprom == nullptr) {
        for (uint16_t absolute_i = firstByte; absolute_i < firstByte + totalBytes; absolute_i++) {
            EEPROM.update(absolute_i, 0);
        }
    } else {
        for (uint16_t absolute_i = firstByte; absolute_i < firstByte + totalBytes; absolute_i++) {
            dummy_eeprom->update(absolute_i, 0);
        }
    }
}