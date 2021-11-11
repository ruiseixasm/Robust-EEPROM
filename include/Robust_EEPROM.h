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

class mathFunctions {
    public:
        static uint16_t max_uint(uint16_t, uint16_t);
        static uint16_t min_uint(uint16_t, uint16_t);
        static uint16_t ceil_log2(uint16_t);
        static uint16_t seed_generator(uint32_t=1000, uint32_t=50);
};


class Dummy_EEPROM {
    private:
        uint8_t *dummy_bytes;
        uint16_t *ttl_bytes;
        uint16_t size;
    public:
        Dummy_EEPROM(uint16_t);
        ~Dummy_EEPROM();
        uint16_t length();
        uint8_t read(uint16_t);
        void write(uint16_t, uint8_t);
        void update(uint16_t, uint8_t);
};


class Robust_EEPROM {
    private:
        Dummy_EEPROM *dummy_eeprom;
        uint16_t totalBytes;
        uint16_t totalControlBytes;
        uint16_t totalDataBytes;
        uint16_t firstByte;
        void initiateEEPROM();
        void offsetright(uint16_t);
        void disablebyte(uint16_t);
    public:
        Robust_EEPROM(uint16_t, uint16_t, Dummy_EEPROM*);
        Robust_EEPROM(uint16_t, uint16_t);
        Robust_EEPROM(Dummy_EEPROM*);
        Robust_EEPROM();
        uint16_t length();
        uint16_t physicallength();
        uint16_t datalength();
        uint16_t controllength();
        uint16_t physicalbyte(uint16_t);
        uint8_t read(uint16_t);
        void write(uint16_t, uint8_t);
        void update(uint16_t, uint8_t);
        void fullreset();

};


#endif // Robust_EEPROM_H_INCLUDED