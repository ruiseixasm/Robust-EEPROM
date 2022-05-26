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
#ifndef Robust_EEPROM_H_INCLUDED
#define Robust_EEPROM_H_INCLUDED

#include <Arduino.h>
#include <EEPROM.h>

class Dummy_EEPROM {
    private:
        uint8_t *dummy_bytes;
        uint16_t *ttl_bytes;
        uint16_t size;
    public:
        Dummy_EEPROM(uint16_t size);
        ~Dummy_EEPROM();
        uint16_t length();
        uint8_t read(uint16_t physical_byte);
        void write(uint16_t physical_byte, uint8_t data);
        void update(uint16_t physical_byte, uint8_t data);
        static uint16_t seed_generator(uint32_t total_duration=1000, uint32_t fragmental_duration=50);
};

class Robust_EEPROM {
    public:
        enum State : uint8_t {depleted, offsetting, available};
    private:
        Dummy_EEPROM *dummy_eeprom = nullptr;
        uint16_t firstByte = 0;
        uint16_t totalBytes;
        uint16_t netBytes;
        uint16_t rightestByte = 0; // Stops new memory allocation when all working Data Bytes become depleted
        void setNetBytes();
        State offsetRight(uint16_t);
        void disableByte(uint16_t);
        uint8_t physicalRead(uint16_t physical_byte);
        void physicalWrite(uint16_t physical_byte, uint8_t data);
        void physicalUpdate(uint16_t physical_byte, uint8_t data);
    public:
        Robust_EEPROM(uint16_t firstByte, uint16_t lengthBytes, Dummy_EEPROM* const dummy_eeprom);
        Robust_EEPROM(Dummy_EEPROM* const dummy_eeprom);
        Robust_EEPROM(uint16_t firstByte, uint16_t lengthBytes);
        Robust_EEPROM();
        uint16_t netLength();
        uint16_t allocatedLength();
        uint16_t totalLength();
        uint16_t controlLength();
        uint16_t dataLength();
        uint16_t physicalByte(uint16_t virtual_byte);
        uint8_t read(uint16_t virtual_byte);
        void write(uint16_t virtual_byte, uint8_t data);
        void update(uint16_t virtual_byte, uint8_t data);
        void fullreset();
};

#endif // Robust_EEPROM_H_INCLUDED