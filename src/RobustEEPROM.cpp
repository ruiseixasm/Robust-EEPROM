#include "RobustEEPROM.h"

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

DummyEEPROM::DummyEEPROM (uint16_t size) {

    dummy_bytes = new uint8_t[size];
    ttl_bytes = new uint16_t[size];

    for (uint16_t b = 0; b < size; b++) {
        dummy_bytes[b] = floor(rand() * 256);
        ttl_bytes[b] = 65535 - floor(rand() * 30000);    // maximum for uint16_t = 65535
    }

}

uint16_t DummyEEPROM::length () {
    return sizeof dummy_bytes;
}

uint8_t DummyEEPROM::read (uint16_t read_byte) {
    return dummy_bytes[read_byte];
}

void DummyEEPROM::write (uint16_t write_byte, uint8_t data) {

    if (ttl_bytes[write_byte] > 0) {
        dummy_bytes[write_byte] = data;
        ttl_bytes[write_byte]--;
    }

}

void DummyEEPROM::update (uint16_t update_byte, uint8_t data) {
    if (read(update_byte) != data)
        write(update_byte, data);
}

RobustEEPROM::RobustEEPROM (uint16_t firstByte=0, uint16_t lengthBytes=EEPROM.length(), DummyEEPROM* const dummy_eeprom = nullptr) {

    uint16_t totalControlBytes;
    uint16_t totalDataBytes;
    uint16_t lastByteBytes;

    this->dummy_eeprom = dummy_eeprom;

    lengthBytes = mathFunctions::min_uint(lengthBytes, EEPROM.length());
    firstByte = mathFunctions::min_uint(firstByte, EEPROM.length() - lengthBytes);
    lastDataByte = firstByte;

    totalControlBytes = ceil(lengthBytes/9) + 2;
    totalDataBytes = lengthBytes - totalControlBytes;

}

RobustEEPROM::RobustEEPROM (DummyEEPROM* const dummy_eeprom) {

    RobustEEPROM(0, dummy_eeprom->length(), dummy_eeprom);    

    // uint16_t totalControlBytes;
    // uint16_t totalDataBytes;
    // uint16_t lastByteBytes;

    // this->dummy_eeprom = dummy_eeprom;

    // firstByte = 0;
    // lastDataByte = firstByte;

    // totalControlBytes = ceil(this->dummy_eeprom->length()/9) + 2;
    // totalDataBytes = this->dummy_eeprom->length() - totalControlBytes;

}

uint16_t RobustEEPROM::length () {

    uint16_t useful_length = 0;
    uint8_t temp_byte = 0;

    if (dummy_eeprom == nullptr) {
        for (uint16_t i = firstByte + totalDataBytes + totalControlBytes - 1; i >= firstByte + totalDataBytes - 1; i--) {
            temp_byte = EEPROM.read(i);
            for (int b = 0; b < 8; b++) {
                useful_length += temp_byte >> b & 0b00000001;
            }
        }
    } else {
        for (uint16_t i = firstByte + totalDataBytes + totalControlBytes - 1; i >= firstByte + totalDataBytes - 1; i--) {
            temp_byte = dummy_eeprom->read(i);
            for (int b = 0; b < 8; b++) {
                useful_length += temp_byte >> b & 0b00000001;
            }
        }
    }

    return useful_length;

}

uint16_t RobustEEPROM::absolutelength () {

    return totalDataBytes + totalControlBytes;

}

uint16_t RobustEEPROM::datalength () {

    return totalDataBytes;

}

uint16_t RobustEEPROM::controllength () {

    return totalControlBytes;

}

uint8_t RobustEEPROM::read (uint16_t read_byte) {

    if (dummy_eeprom == nullptr) {
        /* code */
    } else {

    }

}

void RobustEEPROM::write (uint16_t write_byte, uint8_t data) {

    if (dummy_eeprom == nullptr) {
        /* code */
    } else {

    }

}

void RobustEEPROM::update (uint16_t write_byte, uint8_t data) {

    if (dummy_eeprom == nullptr) {
        /* code */
    } else {

    }

}

uint16_t RobustEEPROM::reset () {

    if (dummy_eeprom == nullptr) {
        /* code */
    } else {

    }

}