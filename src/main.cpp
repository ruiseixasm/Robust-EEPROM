#include "Robust_EEPROM.h"

int data_health = 0;
bool stop = false;

Dummy_EEPROM *dummy_eeprom;
Robust_EEPROM *robust_eeprom;

void setup() {
    Serial.begin(9600);
    Serial.println("Preparing!");
    dummy_eeprom = new Dummy_EEPROM(1024/4);
    robust_eeprom = new Robust_EEPROM(dummy_eeprom);
    Serial.println("");
    Serial.println("Starting!");
}

void loop() {

    if (!stop) {

        for (int i = 0; i < 100; i++) {
            robust_eeprom->update(i, rand() % 20);
        }
    
        if (data_health != (int)floor(100*robust_eeprom->length()/robust_eeprom->datalength())) {

            data_health = (int)floor(100*robust_eeprom->length()/robust_eeprom->datalength());
            Serial.println(robust_eeprom->length());
            Serial.println(robust_eeprom->datalength());
            Serial.print(data_health);
            Serial.println("%");
            if (data_health < 90)
                stop = true;
        }
        
        stop = true;

    }

    // Serial.println(dummy_eeprom->length());
    // delay (1000);

}