#ifndef ROBUSTEEPROM_H_INCLUDED
#define ROBUSTEEPROM_H_INCLUDED

#include <Arduino.h>
#include <EEPROM.h>

class DummyEEPROM {

    

};


class RobustEEPROM {
    private:
        uint8_t *working_bytes;
    public:
        RobustEEPROM(int firstByte=0, int lastByte=EEPROM.length());
        

};


#endif // ROBUSTEEPROM_H_INCLUDED