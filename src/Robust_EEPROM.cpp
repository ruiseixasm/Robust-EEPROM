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



// Dummy_EEPROM Functions ---------------------------------------------------------------------------------

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
        ttl_bytes[b] = (uint16_t)(1000 - rand() % 500); // maximum for uint16_t = 65535
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



// Robust_EEPROM Functions ---------------------------------------------------------------------------------

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
    for (uint16_t control_byte = 0; control_byte < controlLength(); control_byte++) { // loop of physical bytes
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
    for (uint16_t control_byte = 0; control_byte < controlLength(); control_byte++) { // loop of physical bytes
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
    if (virtual_byte < netBytes) {
        if (dummy_eeprom == nullptr)
            return EEPROM.read(physicalByte(virtual_byte));
        else
            return dummy_eeprom->read(physicalByte(virtual_byte));
    } else
        return 0; // For out of scope read data requests
}

void Robust_EEPROM::write (uint16_t virtual_byte, uint8_t data) {
    if (virtual_byte < netBytes) {
        rightestByte = max(virtual_byte, rightestByte); // New data commited to be allocated (preserved)
        uint8_t tryouts = 0;
        do {
            tryouts++;
            if (tryouts > 3) {
                if (offsetRight(virtual_byte) != depleted)
                    tryouts = 1;
                else // Breaks loop when available memory is depleted (avoids infinite loop)
                    break;
            }
            if (dummy_eeprom == nullptr)
                EEPROM.write(physicalByte(virtual_byte), data);
            else
                dummy_eeprom->write(physicalByte(virtual_byte), data);
        } while (read(virtual_byte) != data);
    }
}

void Robust_EEPROM::update (uint16_t virtual_byte, uint8_t data) {
    if (virtual_byte < netBytes) {
        if (read(virtual_byte) != data)
            write(virtual_byte, data);
        else
            rightestByte = max(virtual_byte, rightestByte); // New data commited to be allocated (preserved)
    }

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

Robust_EEPROM::State Robust_EEPROM::offsetRight (uint16_t failed_virtual_byte) {
    State state = offsetting;
    if (allocatedLength() < netLength()) {
        disableByte(failed_virtual_byte); // Starts by disabling the failed byte (data to be replaced anyway) (root failed byte)
        while (state == offsetting) {
            for (uint16_t data_byte = rightestByte; data_byte > failed_virtual_byte; data_byte--) { // loop of virtual bytes
                for (uint8_t tryout = 0; tryout < 3; tryout++) {
                    if (dummy_eeprom == nullptr)
                        EEPROM.update(physicalByte(data_byte), read(data_byte - 1));
                    else
                        dummy_eeprom->update(physicalByte(data_byte), read(data_byte - 1));
                    if (read(data_byte) == read(data_byte - 1))
                        break;
                }
                if (read(data_byte) != read(data_byte - 1)) {
                    if (allocatedLength() < netLength())
                        disableByte(data_byte); // If there is a new failed byte then disable it (already copied) (tail failed byte)
                    else
                        state = depleted;
                    break; // Exit this loop to restart a new one!
                } else if (data_byte - 1 == failed_virtual_byte) {
                    if (allocatedLength() < netLength())
                        state = available;
                    else
                        state = depleted;
                }
            }
        }
    } else
        state = depleted;
    return state;
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
    netBytes--; // Decrements the total amount of available Bytes
}