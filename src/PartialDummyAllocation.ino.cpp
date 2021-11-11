# 1 "C:\\Users\\UTILIZ~1\\AppData\\Local\\Temp\\tmpoazsvw69"
#include <Arduino.h>
# 1 "C:/Users/Utilizador/Documents/GitHub/Robust-EEPROM/src/PartialDummyAllocation.ino"
#include <Robust_EEPROM.h>

int length = 0;
int data_health = 0;
bool stop = false;
Dummy_EEPROM *dummy_eeprom;
Robust_EEPROM *robust_eeprom;
void setup();
void loop();
#line 9 "C:/Users/Utilizador/Documents/GitHub/Robust-EEPROM/src/PartialDummyAllocation.ino"
void setup() {
    Serial.begin(9600);
    Serial.println("Preparing!");

    dummy_eeprom = new Dummy_EEPROM(1024/4);
    robust_eeprom = new Robust_EEPROM(50, 100, dummy_eeprom);
    robust_eeprom->fullreset();

    Serial.println("Starting!");
    Serial.println("");
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

            data_health = (int)(100*(double)robust_eeprom->length()/robust_eeprom->datalength());

            Serial.println("Virtual Addresses");
            Serial.print("    ");
            for (int i = 0; i < 10; i++) {
                Serial.print(i);
                Serial.print(":");
            }
            Serial.println("");
            Serial.println("Physical Addresses");
            Serial.print("    ");
            for (int i = 0; i < 10; i++) {
                Serial.print(robust_eeprom->physicalbyte(i));
                Serial.print(":");
            }
            Serial.println("");
            Serial.println("Data");
            Serial.print("    ");
            for (int i = 0; i < 10; i++) {
                Serial.print(robust_eeprom->read(i));
                Serial.print(":");
            }
            Serial.println("");
            Serial.println("length of datalength of physicallenght = data memory health");
            Serial.print("    ");
            Serial.print(robust_eeprom->length());
            Serial.print(" of ");
            Serial.print(robust_eeprom->datalength());
            Serial.print(" of ");
            Serial.print(robust_eeprom->physicallength());
            Serial.print(" = ");
            Serial.print(data_health);
            Serial.println("%");
            Serial.println("");
            if (data_health < 20) {
                stop = true;
                Serial.println("Finish!");
            }
            length = robust_eeprom->length();
        }

    }

}