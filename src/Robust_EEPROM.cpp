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

Dummy_EEPROM::Dummy_EEPROM (uint16_t size) {

    dummy_bytes = new uint8_t[size];
    ttl_bytes = new uint16_t[size];
    this->size = size;
    srand(size*millis());

    for (uint16_t b = 0; b < size; b++) {
        dummy_bytes[b] = (uint8_t)(rand() % 256);
        ttl_bytes[b] = (uint16_t)(1000 - rand() % 500);    // maximum for uint16_t = 65535
    }

}

uint16_t Dummy_EEPROM::length () {
    return size;
}

uint8_t Dummy_EEPROM::read (uint16_t read_byte) {
    return dummy_bytes[read_byte];
}

void Dummy_EEPROM::write (uint16_t write_byte, uint8_t data) {
    if (ttl_bytes[write_byte] > 0) {
        dummy_bytes[write_byte] = data;
        ttl_bytes[write_byte]--;
    }
}

void Dummy_EEPROM::update (uint16_t update_byte, uint8_t data) {
    if (read(update_byte) != data)
        write(update_byte, data);
    
}

Robust_EEPROM::Robust_EEPROM (uint16_t firstByte, uint16_t lengthBytes, Dummy_EEPROM* const dummy_eeprom) {

    Serial.println("FINAL");

    this->dummy_eeprom = dummy_eeprom;

    if (dummy_eeprom == nullptr) {

        totalBytes = mathFunctions::min_uint(lengthBytes, EEPROM.length());
        this->firstByte = mathFunctions::min_uint(firstByte, EEPROM.length() - lengthBytes);

    } else {

        totalBytes = mathFunctions::min_uint(lengthBytes, dummy_eeprom->length());
        this->firstByte = mathFunctions::min_uint(firstByte, dummy_eeprom->length() - lengthBytes);

    }

    lastDataByte = this->firstByte;

    totalControlBytes = (uint16_t)ceil((double)totalBytes/9) + 2;
    totalDataBytes = totalBytes - totalControlBytes;

}

Robust_EEPROM::Robust_EEPROM (Dummy_EEPROM* const dummy_eeprom) {

    Serial.println("dummy_eeprom");

    this->dummy_eeprom = dummy_eeprom;

    totalBytes = dummy_eeprom->length();

    firstByte = 0;
    lastDataByte = firstByte;

    totalControlBytes = (uint16_t)ceil((double)totalBytes/9) + 2;
    totalDataBytes = totalBytes - totalControlBytes;
    
}

Robust_EEPROM::Robust_EEPROM () {

    Serial.println("EEPROM");

    dummy_eeprom = nullptr;

    totalBytes = EEPROM.length();
    firstByte = 0;
    lastDataByte = firstByte;

    totalControlBytes = (uint16_t)ceil((double)totalBytes/9) + 2;
    totalDataBytes = totalBytes - totalControlBytes;

}

uint16_t Robust_EEPROM::length () {

    uint16_t failed_bytes = 0;
    uint8_t temp_byte = 0;

    if (dummy_eeprom == nullptr) {
        for (uint16_t i = firstByte + absolutelength() - 1; i >= firstByte + datalength() + 2 - 1; i--) {
            temp_byte = EEPROM.read(i);
            for (int b = 0; b < 8; b++) {
                failed_bytes += temp_byte >> b & 0b00000001;
            }
        }
    } else {
        for (uint16_t i = firstByte + absolutelength() - 1; i >= firstByte + datalength() + 2 - 1; i--) {
            temp_byte = dummy_eeprom->read(i);
            for (int b = 0; b < 8; b++) {
                failed_bytes += temp_byte >> b & 0b00000001;
            }
        }
    }

    return totalDataBytes - failed_bytes;

}

uint16_t Robust_EEPROM::absolutelength () {
    return totalDataBytes + totalControlBytes;
}

uint16_t Robust_EEPROM::datalength () {
    return totalDataBytes;
}

uint16_t Robust_EEPROM::controllength () {
    return totalControlBytes;
}

uint16_t Robust_EEPROM::lastdatabyte () {
    return lastDataByte;
}

uint16_t Robust_EEPROM::absolutebyte (uint16_t relative_byte) {

    uint16_t absolute_byte = 0;
    uint8_t temp_byte = 0;

    // relative_byte += offsetDataByte;
    
    if (dummy_eeprom == nullptr) {
        for (uint16_t i = firstByte + absolutelength() - 1; i >= firstByte + datalength() + 2 - 1; i--) {
            temp_byte = EEPROM.read(i);
            for (int b = 0; b < 8; b++) {
                relative_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
                if (relative_byte == 0) {
                    return absolute_byte;
                } else {
                    relative_byte--;
                    absolute_byte++;
                }

            }
        }
    } else {
        for (uint16_t i = firstByte + absolutelength() - 1; i >= firstByte + datalength() + 2 - 1; i--) {
            temp_byte = dummy_eeprom->read(i);
            for (int b = 0; b < 8; b++) {
                relative_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
                if (relative_byte == 0) {
                    return absolute_byte;
                } else {
                    relative_byte--;
                    absolute_byte++;
                }

            }
        }
    }
    return absolute_byte;

}

uint8_t Robust_EEPROM::read (uint16_t read_byte) {

    read_byte = mathFunctions::max_uint(read_byte, firstByte);
    read_byte = mathFunctions::min_uint(read_byte, firstByte + totalDataBytes - 1);
    if (dummy_eeprom == nullptr) {
        return EEPROM.read(absolutebyte(read_byte));
    } else {
        return dummy_eeprom->read(absolutebyte(read_byte));
    }

}

void Robust_EEPROM::write (uint16_t write_byte, uint8_t data) {

    int tryouts = 0;
    write_byte = mathFunctions::max_uint(write_byte, firstByte);
    write_byte = mathFunctions::min_uint(write_byte, firstByte + totalDataBytes - 1);
    do {
        if (dummy_eeprom == nullptr) {
            EEPROM.write(absolutebyte(write_byte), data);
        } else {
            dummy_eeprom->write(absolutebyte(write_byte), data);
        }
        if (tryouts == 5) {
            // offsetright(write_byte);
            disablebyte(write_byte);
            tryouts = 0;
        }
        tryouts++;
    } while (read(write_byte) != data);

    lastDataByte = mathFunctions::max_uint(write_byte, lastDataByte);

}

void Robust_EEPROM::update (uint16_t update_byte, uint8_t data) {

    int tryouts = 0;
    update_byte = mathFunctions::max_uint(update_byte, firstByte);
    update_byte = mathFunctions::min_uint(update_byte, firstByte + totalDataBytes - 1);
    do {
        if (dummy_eeprom == nullptr) {
            EEPROM.update(absolutebyte(update_byte), data);
        } else {
            dummy_eeprom->update(absolutebyte(update_byte), data);
        }
        if (tryouts == 5) {
            offsetright(update_byte);
            disablebyte(update_byte);
            tryouts = 0;
        }
        tryouts++;
    } while (read(update_byte) != data);

    lastDataByte = mathFunctions::max_uint(update_byte, lastDataByte);

}

void Robust_EEPROM::offsetright (uint16_t actual_byte) {

    int tryouts;
    offsetDataByte++;
    for (uint16_t i = lastDataByte + offsetDataByte; i > actual_byte + 1; i--) {
        tryouts = 0;
        do {

            // if (true) {
            
            //     Serial.print(offsetDataByte);
            //     Serial.print(" // ");
            //     Serial.print(actual_byte);
            //     Serial.print(":");
            //     Serial.print(absolutebyte(actual_byte));
            //     Serial.print(" - ");
            //     Serial.print(i - 1);
            //     Serial.print(":");
            //     Serial.print(i);
            //     Serial.print(" ~ ");
            //     Serial.print(absolutebyte(i - 1));
            //     Serial.print(":");
            //     Serial.print(absolutebyte(i));
            //     Serial.print(" = ");
            //     Serial.print(read(i - 1));
            //     Serial.print(":");
            //     Serial.println(read(i));
            
            // }
            
            if (dummy_eeprom == nullptr) {
                EEPROM.update(absolutebyte(i), read(i - 1));
            } else {
                dummy_eeprom->update(absolutebyte(i), read(i - 1));
            }
            if (tryouts == 5) {
                offsetright(i);
                disablebyte(i);
                tryouts = 0;
            }
            tryouts++;
            // delay(1000);
        } while (read(i) != read(i - 1));
    }
    offsetDataByte--;
}

void Robust_EEPROM::disablebyte (uint16_t relative_byte) {

    uint8_t temp_byte = 0;
    
    if (dummy_eeprom == nullptr) {
        for (uint16_t i = firstByte + absolutelength() - 1; i >= firstByte + datalength() + 2 - 1; i--) {
            temp_byte = EEPROM.read(i);
            for (int b = 0; b < 8; b++) {
                relative_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
                if (relative_byte == 0) {
                    temp_byte |= 0b00000001 << b;
                    EEPROM.write(i, temp_byte);
                    return;
                } else {
                    relative_byte--;
                }
            }
        }
    } else {
        for (uint16_t i = firstByte + absolutelength() - 1; i >= firstByte + datalength() + 2 - 1; i--) {
            temp_byte = dummy_eeprom->read(i);
            for (int b = 0; b < 8; b++) {
                relative_byte += temp_byte >> b & 0b00000001; // offsets all failed bytes
                if (relative_byte == 0) {
                    temp_byte |= 0b00000001 << b;
                    dummy_eeprom->write(i, temp_byte);
                    return;
                } else {
                    relative_byte--;
                }
            }
        }
    }

}

void Robust_EEPROM::fullreset () {

    if (dummy_eeprom == nullptr) {
        for (uint16_t i = firstByte; i < firstByte + absolutelength(); i++) {
            EEPROM.write(i, 0);
        }
    } else {
        for (uint16_t i = firstByte; i < firstByte + absolutelength(); i++) {
            dummy_eeprom->write(i, 0);
        }
    }

}