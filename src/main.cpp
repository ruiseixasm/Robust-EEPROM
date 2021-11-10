#include "Robust_EEPROM.h"

int length = 0;
int data_health = 0;
bool stop = false;

Dummy_EEPROM *dummy_eeprom;
Robust_EEPROM *robust_eeprom;

void setup() {
    Serial.begin(9600);
    Serial.println("Preparing!");
    dummy_eeprom = new Dummy_EEPROM(1024/4);
    // robust_eeprom = new Robust_EEPROM(dummy_eeprom);
    robust_eeprom = new Robust_EEPROM(50, 100, dummy_eeprom);
    robust_eeprom->fullreset();
    Serial.println("Starting!");
    Serial.println("");
    delay(1000);
    for (int i = 0; i < 10; i++) {
        robust_eeprom->update(i, rand() % 256);
    }
}

void loop() {

    if (!stop) {

        for (int i = 0; i < 5; i++) {
            robust_eeprom->update(i, rand() % 256);
        }
    
        if (length != robust_eeprom->length()) {

            data_health = (int)floor(100*robust_eeprom->length()/robust_eeprom->datalength());
            for (int i = 0; i < 10; i++) {
                Serial.print(i);
                Serial.print(":");
            }
            Serial.println("");
            for (int i = 0; i < 10; i++) {
                Serial.print(robust_eeprom->absolutebyte(i));
                Serial.print(":");
            }
            Serial.println("");
            for (int i = 0; i < 10; i++) {
                Serial.print(robust_eeprom->read(i));
                Serial.print(":");
            }
            Serial.println("");
            Serial.print(robust_eeprom->lastdatabyte());
            Serial.print(" of ");
            Serial.print(robust_eeprom->length());
            Serial.print(" of ");
            Serial.print(robust_eeprom->datalength());
            Serial.print(" of ");
            Serial.print(robust_eeprom->absolutelength());
            Serial.print(" = ");
            Serial.print(data_health);
            Serial.println("%");
            Serial.println("");
            if (data_health < 95) {  // Last Percentage %
                stop = true;
                Serial.println("--------------------------");
                Serial.println(robust_eeprom->length());
                Serial.println(robust_eeprom->datalength());
                Serial.println(robust_eeprom->lastdatabyte());
                Serial.println(robust_eeprom->absolutebyte(robust_eeprom->lastdatabyte()));
            }
            
            length = robust_eeprom->length();
        }
        
    }

}