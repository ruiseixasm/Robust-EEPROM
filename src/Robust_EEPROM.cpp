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

#pragma region Dummy_EEPROM

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

#pragma endregion Dummy_EEPROM


// Main EEPROM Functions (typical usage) --------------------------------------------------------
#pragma region Robust_EEPROM

Robust_EEPROM::Robust_EEPROM (uint16_t firstByte, uint16_t lengthBytes, Dummy_EEPROM* const dummy_eeprom) {
    this->dummy_eeprom = dummy_eeprom;
    this->firstByte = min(firstByte, dummy_eeprom->length() - 1);
    totalBytes = min(lengthBytes, dummy_eeprom->length() - this->firstByte);
    setNetBytes();
}

Robust_EEPROM::Robust_EEPROM (Dummy_EEPROM* const dummy_eeprom) {
    this->dummy_eeprom = dummy_eeprom;
    totalBytes = dummy_eeprom->length();
    setNetBytes();
}

Robust_EEPROM::Robust_EEPROM (uint16_t firstByte, uint16_t lengthBytes) {
    this->firstByte = min(firstByte, EEPROM.length() - 1);
    totalBytes = min(lengthBytes, EEPROM.length() - this->firstByte);
    setNetBytes();
}

Robust_EEPROM::Robust_EEPROM () {
    totalBytes = EEPROM.length();
    setNetBytes();
}

void Robust_EEPROM::setNetBytes () {
    uint16_t disabled_bytes = 0;
    uint8_t control_data = 0;
    for (uint16_t control_byte = 0; control_byte < controlLength(); control_byte++) { // physical for loop
        if (dummy_eeprom == nullptr)
            control_data = EEPROM.read(firstByte + dataLength() + control_byte);
        else
            control_data = dummy_eeprom->read(firstByte + dataLength() + control_byte);
        for (uint8_t control_bit = 0; control_bit < min(dataLength() - 8*control_byte, 8); control_bit++)
            disabled_bytes += control_data >> control_bit & 0b00000001;
    }
    netBytes = dataLength() - disabled_bytes;
}

uint16_t Robust_EEPROM::netLength () {
    return netBytes;
}

uint16_t Robust_EEPROM::allocatedLength () {
    return rightestByte + 1;
}

uint16_t Robust_EEPROM::totalLength () {
    return totalBytes;
}

uint16_t Robust_EEPROM::controlLength () {
    return (uint16_t)ceil((double)totalBytes/9);
}

uint16_t Robust_EEPROM::dataLength () {
    return totalBytes - controlLength();
}

uint16_t Robust_EEPROM::physicalByte (uint16_t virtual_byte) {
    uint16_t physical_byte = 0;
    uint8_t control_data;
    for (uint16_t control_byte = 0; control_byte < controlLength(); control_byte++) { // physical for loop
        if (dummy_eeprom == nullptr)
            control_data = EEPROM.read(firstByte + dataLength() + control_byte);
        else
            control_data = dummy_eeprom->read(firstByte + dataLength() + control_byte);
        for (uint8_t control_bit = 0; control_bit < min(dataLength() - 8*control_byte, 8); control_bit++) {
            virtual_byte += control_data >> control_bit & 0b00000001; // jumps all disabled bytes
            if (virtual_byte == 0 || physical_byte == dataLength() - 1) {
                return firstByte + physical_byte;
            } else {
                virtual_byte--;
                physical_byte++;
            }
        }
    }
    return firstByte + physical_byte;
}

uint8_t Robust_EEPROM::read (uint16_t virtual_byte) {
    if (dummy_eeprom == nullptr)
        return EEPROM.read(physicalByte(virtual_byte));
    else
        return dummy_eeprom->read(physicalByte(virtual_byte));
}

void Robust_EEPROM::write (uint16_t virtual_byte, uint8_t data) {
    rightestByte = max(virtual_byte, rightestByte);
    uint8_t tryouts = 0;
    do {
        if (dummy_eeprom == nullptr)
            EEPROM.write(physicalByte(virtual_byte), data);
        else
            dummy_eeprom->write(physicalByte(virtual_byte), data);
        if (tryouts > 5 && read(virtual_byte) != data) {
            if (allocatedLength() < netLength()) {
                offsetRight(virtual_byte);
                rightestByte--; // Needs to remove the +1 for each Offset operation
                disableByte(virtual_byte);
                tryouts = 0;
            } else { // Decrements netBytes and Breaks loop when available memory depleted
                netBytes = rightestByte;
                break;
            }
        }
        tryouts++;
    } while (read(virtual_byte) != data);
}

void Robust_EEPROM::update (uint16_t virtual_byte, uint8_t data) {
    if (read(virtual_byte) != data)
        write(virtual_byte, data);
}

void Robust_EEPROM::fullreset () {
    if (dummy_eeprom == nullptr)
        for (uint16_t physical_byte = firstByte; physical_byte < firstByte + totalBytes; physical_byte++)
            EEPROM.update(physical_byte, 0);
    else
        for (uint16_t physical_byte = firstByte; physical_byte < firstByte + totalBytes; physical_byte++)
            dummy_eeprom->update(physical_byte, 0);
    netBytes = dataLength(); // Resets the total amount of available Bytes to maximum
}

void Robust_EEPROM::offsetRight (uint16_t failed_virtual_byte) {
    // The failed_virtual_byte contains outdated data so no offset needed for it,
    // for the situation resulted from an upper offset call (recursive) then
    // the new failed_virtual_byte offset was already performed by the previous iteration (kept).
    for (uint16_t data_byte = rightestByte; data_byte > failed_virtual_byte; data_byte--) // virtual for loop
        update(data_byte + 1, read(data_byte)); // Increments the rightestByte, this will be decremented in the write function
}

void Robust_EEPROM::disableByte (uint16_t failed_virtual_byte) {

    uint16_t data_byte = physicalByte(failed_virtual_byte) - firstByte;
    uint8_t control_data;
    // physical processing on control Bytes
    if (dummy_eeprom == nullptr) {
        control_data = EEPROM.read(firstByte + dataLength() + (uint16_t)(data_byte/8)) | 0b00000001 << data_byte % 8;
        EEPROM.update(firstByte + dataLength() + (uint16_t)(data_byte/8), control_data);
    } else {
        control_data = dummy_eeprom->read(firstByte + dataLength() + (uint16_t)(data_byte/8)) | 0b00000001 << data_byte % 8;
        dummy_eeprom->update(firstByte + dataLength() + (uint16_t)(data_byte/8), control_data);
    }
    netBytes--; // Decrements the total amount of still enabled Bytes
}

#pragma endregion Robust_EEPROM