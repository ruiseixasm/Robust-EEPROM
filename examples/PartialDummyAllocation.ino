#include <Robust_EEPROM.h>

int test_array_length = 10;
int *starting_data_array;  
int length = 0;
int data_health = 0;
Dummy_EEPROM *dummy_eeprom;
Robust_EEPROM *robust_eeprom;
enum Test {testing, result, stop};
Test test = testing;

void setup() {
    Serial.begin(9600);
    Serial.println("Preparing!");

    dummy_eeprom = new Dummy_EEPROM(1024/4); // Avoid using all 1024 board RAM memory
    robust_eeprom = new Robust_EEPROM(50, 100, dummy_eeprom); // PARTIAL DUMMY ALLOCATION
    robust_eeprom->fullreset();

    Serial.println("Starting!");
    Serial.println("");
    for (int i = 0; i < test_array_length; i++)
        robust_eeprom->update(i, rand() % 256);
        
    starting_data_array = new int[test_array_length];
    for (int i = 0; i < test_array_length; i++)
        starting_data_array[i] = robust_eeprom->read(i);

}

void loop() {

    if (test == testing) {

        for (int i = 0; i < test_array_length / 2; i++)
            robust_eeprom->update(i, rand() % 256);
    
        if (length != robust_eeprom->netLength()) {

            data_health = (int)(100*(double)robust_eeprom->netLength()/robust_eeprom->dataLength());

            Serial.println("Virtual Addresses");
            Serial.print("    ");
            for (int i = 0; i < test_array_length; i++) {
                Serial.print(i);
                Serial.print(":");
            }
            Serial.println("");
            Serial.println("Physical Addresses");
            Serial.print("    ");
            for (int i = 0; i < test_array_length; i++) {
                Serial.print(robust_eeprom->physicalByte(i));
                Serial.print(":");
            }
            Serial.println("");
            Serial.println("Data");
            Serial.print("    ");
            for (int i = 0; i < test_array_length; i++) {
                Serial.print(robust_eeprom->read(i));
                Serial.print(":");
            }
            Serial.println("");
            Serial.println("length of datalength of physicallenght = data memory health");
            Serial.print("    ");
            Serial.print(robust_eeprom->netLength());
            Serial.print(" of ");
            Serial.print(robust_eeprom->dataLength());
            Serial.print(" of ");
            Serial.print(robust_eeprom->physicalLength());
            Serial.print(" = ");
            Serial.print(data_health);
            Serial.println("%");
            Serial.println("");
            if (data_health < 20) {  // Last Percentage %
                test = result;
                Serial.println("Finish!");
            }
            length = robust_eeprom->netLength();
        }
        
    } else if (test == result) {

        // Check memory integrity
        bool passed = true;
        for (int i = test_array_length / 2; i < test_array_length; i++)
            if (starting_data_array[i] != robust_eeprom->read(i)) {
                passed = false;
                break;
            }

        Serial.println("---------------------------------------");
        if (passed)
            Serial.println("PASSED!");
        else
            Serial.println("FAIL!");
                
        test = stop;
    }
}