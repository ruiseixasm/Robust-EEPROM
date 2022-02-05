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

uint16_t Dummy_EEPROM::seed_generator (uint32_t total_duration, uint32_t fragmental_duration) {

    uint16_t magic_number = 0;
    uint32_t start_stamp = millis();
    uint32_t actual_stamp = start_stamp;

    while (labs(millis() - start_stamp) < total_duration) {
        magic_number++;
        if (labs(millis() - actual_stamp) >= fragmental_duration) {
            if (analogRead(14) % 2 == 0) // pin 14 is equivalent to A0
                magic_number /= 2;
            else
                magic_number *= 2;
            actual_stamp = millis();
        }
    }
    return magic_number;
}

Dummy_EEPROM::Dummy_EEPROM (uint16_t size) {

    dummy_bytes = new uint8_t[size];
    ttl_bytes = new uint16_t[size];
    this->size = size;
    srand(seed_generator());

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

    this->firstByte = min(firstByte, dummy_eeprom->length() - 1);
    totalBytes = min(lengthBytes, dummy_eeprom->length() - this->firstByte);
}

Robust_EEPROM::Robust_EEPROM (Dummy_EEPROM* const dummy_eeprom) {

    this->dummy_eeprom = dummy_eeprom;

    firstByte = 0;
    totalBytes = dummy_eeprom->length();
}


// Main Functions (most common)

Robust_EEPROM::Robust_EEPROM (uint16_t firstByte, uint16_t lengthBytes) {

    this->firstByte = min(firstByte, EEPROM.length() - 1);
    totalBytes = min(lengthBytes, EEPROM.length() - this->firstByte);
}

Robust_EEPROM::Robust_EEPROM () {

    firstByte = 0;
    totalBytes = EEPROM.length();
}

uint16_t Robust_EEPROM::netLength () {

    uint16_t disabled_bytes = 0;
    uint8_t temp_byte = 0;

    if (dummy_eeprom == nullptr) {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + dataLength() - 1; control_i--) {
            temp_byte = EEPROM.read(control_i);
            for (int bit_byte = 0; bit_byte < 8; bit_byte++)
                disabled_bytes += temp_byte >> bit_byte & 0b00000001;
        }
    } else {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + dataLength() - 1; control_i--) {
            temp_byte = dummy_eeprom->read(control_i);
            for (int bit_byte = 0; bit_byte < 8; bit_byte++)
                disabled_bytes += temp_byte >> bit_byte & 0b00000001;
        }
    }
    return dataLength() - disabled_bytes;
}

uint16_t Robust_EEPROM::physicalLength () {
    return totalBytes;
}

uint16_t Robust_EEPROM::controlLength () { // totalControlBytes
    return (uint16_t)ceil((double)totalBytes/9);
}

uint16_t Robust_EEPROM::dataLength () { // totalDataBytes
    return totalBytes - controlLength();
}

uint16_t Robust_EEPROM::physicalByte (uint16_t virtual_byte) {

    uint16_t physical_byte = 0;
    uint8_t temp_byte;

    if (dummy_eeprom == nullptr) {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + dataLength() - 1; control_i--) {
            temp_byte = EEPROM.read(control_i);
            for (int bit_byte = 0; bit_byte < 8; bit_byte++) {
                virtual_byte += temp_byte >> bit_byte & 0b00000001; // jumps all disabled bytes
                if (virtual_byte == 0 || physical_byte == dataLength() - 1) {
                    return physical_byte + firstByte;
                } else {
                    virtual_byte--;
                    physical_byte++;
                }
            }
        }
    } else {
        for (uint16_t control_i = firstByte + totalBytes - 1; control_i > firstByte + dataLength() - 1; control_i--) {
            temp_byte = dummy_eeprom->read(control_i);
            for (int bit_byte = 0; bit_byte < 8; bit_byte++) {
                virtual_byte += temp_byte >> bit_byte & 0b00000001; // jumps all disabled bytes
                if (virtual_byte == 0 || physical_byte == dataLength() - 1) {
                    return physical_byte + firstByte;
                } else {
                    virtual_byte--;
                    physical_byte++;
                }
            }
        }
    }
    return firstByte + physical_byte;
}

uint8_t Robust_EEPROM::read (uint16_t virtual_byte) {

    if (virtual_byte < dataLength()) {
        if (dummy_eeprom == nullptr)
            return EEPROM.read(physicalByte(virtual_byte));
        else
            return dummy_eeprom->read(physicalByte(virtual_byte));
    } else
        return 0;
}

void Robust_EEPROM::write (uint16_t virtual_byte, uint8_t data) {

    if (virtual_byte < dataLength()) {
        int tryouts = 0;
        do {
            if (dummy_eeprom == nullptr)
                EEPROM.write(physicalByte(virtual_byte), data);
            else
                dummy_eeprom->write(physicalByte(virtual_byte), data);
            if (tryouts > 5 && read(virtual_byte) != data) {
                offsetright(virtual_byte);
                disablebyte(virtual_byte);
                tryouts = 0;
            }
            tryouts++;
        } while (read(virtual_byte) != data);
    }
}

void Robust_EEPROM::update (uint16_t virtual_byte, uint8_t data) {

    if (read(virtual_byte) != data)
        write(virtual_byte, data);
}

void Robust_EEPROM::offsetright (uint16_t failed_virtual_byte) {
    // The failed_virtual_byte contains outdated data so no offset needed for it,
    // for the situation resulted from an upper offset call (recursive) then
    // the new failed_virtual_byte offset was already performed by the previous iteration (kept).
    for (uint16_t data_i = dataLength() - 1; data_i > failed_virtual_byte + 1; data_i--)
        update(data_i, read(data_i - 1));
}

void Robust_EEPROM::disablebyte (uint16_t virtual_byte) {

    uint16_t physical_byte = physicalByte(virtual_byte);
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
    if (dummy_eeprom == nullptr)
        for (uint16_t absolute_i = firstByte; absolute_i < firstByte + totalBytes; absolute_i++)
            EEPROM.update(absolute_i, 0);
    else
        for (uint16_t absolute_i = firstByte; absolute_i < firstByte + totalBytes; absolute_i++)
            dummy_eeprom->update(absolute_i, 0);
}