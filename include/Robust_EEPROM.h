#ifndef Robust_EEPROM_H_INCLUDED
#define Robust_EEPROM_H_INCLUDED

#include <Arduino.h>
#include <EEPROM.h>

class mathFunctions {
    public:
        static uint16_t max_uint(uint16_t, uint16_t);
        static uint16_t min_uint(uint16_t, uint16_t);
        static uint16_t ceil_log2(uint16_t);
};


class Dummy_EEPROM {
    private:
        uint8_t *dummy_bytes;
        uint16_t *ttl_bytes;
        uint16_t size;
    public:
        Dummy_EEPROM(uint16_t);
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
        uint16_t lastDataByte;
    public:
        Robust_EEPROM(uint16_t, uint16_t, Dummy_EEPROM*);
        Robust_EEPROM(Dummy_EEPROM*);
        Robust_EEPROM();
        uint16_t length();
        uint16_t absolutelength();
        uint16_t datalength();
        uint16_t controllength();
        uint16_t absolutebyte(uint16_t);
        uint8_t read(uint16_t);
        void write(uint16_t, uint8_t);
        void update(uint16_t, uint8_t);
        void fullreset();

};


#endif // Robust_EEPROM_H_INCLUDED