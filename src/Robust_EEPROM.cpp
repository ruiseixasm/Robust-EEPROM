/*
  Robust_EEPROM.h - Robust_EEPROM library
  Original Copyright (c) 2021 Rui Seixas Monteiro. All right reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.
*/
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
            if (analogRead(14) % 2 == 0) { // pin 14 is equivalent to A0
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

Dummy_EEPROM::~Dummy_EEPROM () {
    delete[] dummy_bytes;
    delete[] ttl_bytes;
    dummy_bytes = nullptr;
    ttl_bytes = nullptr;
}

uint16_t Dummy_EEPROM::length () {
    return size;
}

uint8_t Dummy_EEPROM::read (uint16_t physical_byte) {
    return dummy_bytes[physical_byte];
}

void Dummy_EEPROM::write (uint16_t physical_byte, uint8_t data) {
    if (ttl_bytes[physical_byte] > 0) {
        dummy_bytes[physical_byte] = data;
        ttl_bytes[physical_byte]--;
    }
}

void Dummy_EEPROM::update (uint16_t physical_byte, uint8_t data) {
    if (read(physical_byte) != data)
        write(physical_byte, data);
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
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + totalDataBytes - 1; control_i--) {
            temp_byte = EEPROM.read(control_i);
            for (int b = 0; b < 8; b++) {
                failed_bytes += temp_byte >> b & 0b00000001;
            }
        }
    } else {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + totalDataBytes - 1; control_i--) {
            temp_byte = dummy_eeprom->read(control_i);
            for (int b = 0; b < 8; b++) {
                failed_bytes += temp_byte >> b & 0b00000001;
            }
        }
    }
    return totalDataBytes - failed_bytes;
}

uint16_t Robust_EEPROM::physicallength () {
    return totalBytes;
}

uint16_t Robust_EEPROM::datalength () {
    return totalDataBytes;
}

uint16_t Robust_EEPROM::controllength () {
    return totalControlBytes;
}

uint16_t Robust_EEPROM::physicalbyte (uint16_t virtual_byte) {

    uint16_t physical_byte = 0;
    uint8_t temp_byte;

    if (dummy_eeprom == nullptr) {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + totalDataBytes - 1; control_i--) {
            temp_byte = EEPROM.read(control_i);
            for (int b = 0; b < 8; b++) {
                virtual_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
                if (virtual_byte == 0 || physical_byte == totalDataBytes - 1) {
                    return physical_byte + firstByte;
                } else {
                    virtual_byte--;
                    physical_byte++;
                }

            }
        }
    } else {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + totalDataBytes - 1; control_i--) {
            temp_byte = dummy_eeprom->read(control_i);
            for (int b = 0; b < 8; b++) {
                virtual_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
                if (virtual_byte == 0 || physical_byte == totalDataBytes - 1) {
                    return physical_byte + firstByte;
                } else {
                    virtual_byte--;
                    physical_byte++;
                }

            }
        }
    }
    return physical_byte + firstByte;
}

uint8_t Robust_EEPROM::read (uint16_t virtual_byte) {

    virtual_byte = mathFunctions::min_uint(virtual_byte, totalDataBytes - 1);
    if (dummy_eeprom == nullptr) {
        return EEPROM.read(physicalbyte(virtual_byte));
    } else {
        return dummy_eeprom->read(physicalbyte(virtual_byte));
    }

}

void Robust_EEPROM::write (uint16_t virtual_byte, uint8_t data) {

    virtual_byte = mathFunctions::min_uint(virtual_byte, totalDataBytes - 1);
    int tryouts = 0;
    do {
        if (dummy_eeprom == nullptr) {
            EEPROM.write(physicalbyte(virtual_byte), data);
        } else {
            dummy_eeprom->write(physicalbyte(virtual_byte), data);
        }
        if (tryouts == 5) {
            offsetright(virtual_byte);
            disablebyte(virtual_byte);
            tryouts = 0;
        }
        tryouts++;
    } while (read(virtual_byte) != data);

}

void Robust_EEPROM::update (uint16_t virtual_byte, uint8_t data) {

    if (read(virtual_byte) != data)
        write(virtual_byte, data);

}

void Robust_EEPROM::offsetright (uint16_t virtual_byte) {
    // The virtual_byte is destined to be overwritten so no offset needed for it,
    // for the situation when it's called from a previous offset then
    // the virtual_byte offset was already performed by the previous offset.
    for (uint16_t data_i = totalDataBytes - 1; data_i > virtual_byte + 1; data_i--) {
        update(data_i, read(data_i - 1));
    }
}

void Robust_EEPROM::disablebyte (uint16_t virtual_byte) {

    uint16_t physical_byte = physicalbyte(virtual_byte);
    uint8_t temp_byte;
    
    if (dummy_eeprom == nullptr) {
        temp_byte = EEPROM.read(firstByte + totalBytes - 1 - (uint16_t)((physical_byte - firstByte)/8));
        temp_byte |= 0b00000001 << (physical_byte - firstByte) % 8;
        EEPROM.write(firstByte + totalBytes - 1 - (uint16_t)((physical_byte - firstByte)/8), temp_byte);
    } else {
        temp_byte = dummy_eeprom->read(firstByte + totalBytes - 1 - (uint16_t)((physical_byte - firstByte)/8));
        temp_byte |= 0b00000001 << (physical_byte - firstByte) % 8;
        dummy_eeprom->write(firstByte + totalBytes - 1 - (uint16_t)((physical_byte - firstByte)/8), temp_byte);
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