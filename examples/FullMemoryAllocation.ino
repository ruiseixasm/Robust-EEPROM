#include <Robust_EEPROM.h>

int length = 0;
int data_health = 0;
bool stop = false;
Robust_EEPROM *robust_eeprom;

void setup() {
    Serial.begin(9600);
    Serial.println("Preparing!");

    robust_eeprom = new Robust_EEPROM(); // FULL MEMORY ALLOCATION
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

            Serial.println("Virtual Adresses");
            Serial.print("    ");
            for (int i = 0; i < 10; i++) {
                Serial.print(i);
                Serial.print(":");
            }
            Serial.println("");
            Serial.println("Physical Adresses");
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
            if (data_health < 20)  // Last Percentage %
                stop = true;
            
            length = robust_eeprom->length();
        }
        
    }

}