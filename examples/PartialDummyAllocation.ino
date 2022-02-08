#include <Robust_EEPROM.h>

int test_array_length = 10;
int *starting_data_array;  
int length = 0;
int data_health = 0;
Dummy_EEPROM *dummy_eeprom;
Robust_EEPROM *robust_eeprom;
enum Test : uint8_t {testing, result, stop};
Test test = testing;
enum Step : uint8_t {test1, test2, test3};
Step step = test1;
bool print = false;
bool passed = true;

void setup() {
    Serial.begin(9600);
    Serial.println("Preparing!");

    // COMMENT AND UNCOMMENT ACCORDINGLY TO USAGE SCENARIO
    //********************************************************************************
    // EEPROM ALLOCATION (NORMAL SCENARIO)
    // robust_eeprom = new Robust_EEPROM(); // FULL ALLOCATION
    // robust_eeprom = new Robust_EEPROM(50, 100); // PARTIAL ALLOCATION
    // DUMMY ALLOCATION (TEST SCENARIO)
    dummy_eeprom = new Dummy_EEPROM(1024/4); // Avoid using all 1024 board RAM memory
    // robust_eeprom = new Robust_EEPROM(dummy_eeprom); // FULL ALLOCATION
    robust_eeprom = new Robust_EEPROM(50, 100, dummy_eeprom); // PARTIAL ALLOCATION
    //********************************************************************************
    
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

        if (length != robust_eeprom->netLength() || print) {

            print = false;
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
            Serial.println("(Allocation) of (Net Length) of (Data Length) of (Total Lenght) = (Data Memory Health)");
            Serial.print("    ");
            Serial.print(robust_eeprom->allocatedLength());
            Serial.print(" of ");
            Serial.print(robust_eeprom->netLength());
            Serial.print(" of ");
            Serial.print(robust_eeprom->dataLength());
            Serial.print(" of ");
            Serial.print(robust_eeprom->totalLength());
            Serial.print(" = ");
            Serial.print(data_health);
            Serial.println("%");
            Serial.println("");
            // Use '||' to stop at 20% or '&&' to do the full available memory test
            if (data_health < 20 && robust_eeprom->allocatedLength() == robust_eeprom->netLength()) {
                test = result;
                Serial.println("Finish!");
            }
            // Does a full reset at 50% to check reseting memmory
            if (data_health < 30 && step == test1) {
                step = test2;
                robust_eeprom->fullreset();
                for (int i = 0; i < test_array_length; i++)
                    robust_eeprom->update(i, starting_data_array[i]);
                // Check memory integrity
                Serial.println("Check memory integrity:");
                for (int i = 0; i < test_array_length; i++) {
                    if (starting_data_array[i] != robust_eeprom->read(i)) {
                        passed = false;
                        Serial.println("Test 1: Failed rebuild after full reset!");
                        break;
                    }
                }
                if (passed == true)
                    Serial.println("Test 1: Passed rebuild after full reset!");
                print = true;
            }
            length = robust_eeprom->netLength();
        }
        
        if (!print)
            for (int i = 0; i < test_array_length / 2; i++)
                robust_eeprom->update(i, rand() % 256);
    
    } else if (test == result) {

        // Check memory integrity
        for (int i = test_array_length / 2; i < test_array_length; i++) {
            if (starting_data_array[i] != robust_eeprom->read(i)) {
                Serial.println("Test 2: Failed preservation of unchanged data!");
                passed = false;
                break;
            }
        }
        Serial.println("-------------------------------------------------------------------------------");
        if (passed)
            Serial.println("PASSED!");
        else
            Serial.println("FAIL!");
                
        test = stop;
    }
}