#ifndef ROBUSTEEPROM_H_INCLUDED
#define ROBUSTEEPROM_H_INCLUDED

#include <Arduino.h>
#include <EEPROM.h>

class mathFunctions {
    public:
        static uint16_t max_uint(uint16_t, uint16_t);
        static uint16_t min_uint(uint16_t, uint16_t);
        static uint16_t ceil_log2(uint16_t);

};

class DummyEEPROM {
    private:
        uint8_t *dummy_bytes;
        uint16_t *ttl_bytes;
    public:
        DummyEEPROM(uint16_t);
        uint16_t length();
        uint8_t read(uint16_t);
        void write(uint16_t, uint8_t);
        void update(uint16_t, uint8_t);
};


class RobustEEPROM {
    private:
        DummyEEPROM *dummy_eeprom;
        uint8_t *working_bytes;
        uint16_t totalControlBytes;
        uint16_t totalDataBytes;
        uint16_t firstByte;
        uint16_t lastDataByte;
    public:
        RobustEEPROM(uint16_t, uint16_t, DummyEEPROM*);
        uint16_t length();
        uint8_t read(uint16_t);
        void update(uint16_t, uint8_t);
        uint16_t reset();

};


#endif // ROBUSTEEPROM_H_INCLUDED