## **Robust-EEPROM Library V2.1.0** for Arduino
**Written by:** _Rui Seixas Monteiro_.

## Installation
Create a new folder called "Robust_EEPROM" under the folder named "libraries" in your Arduino sketchbook folder.
Create the folder "libraries" in case it does not exist yet. Place all the files in the "Robust_EEPROM" folder.

## Import
To use the library in your own sketch, select it from *Sketch > Import Library*.

## What is Robust_EEPROM
The Robust_EEPROM library allows the use of unallocated bytes on an EEPROM memory as redundant memory for failed allocated bytes. It does this by using 8/9ths as data bytes and 1/9th as control bytes of the total physical memory, these control bytes are bit markers of failed data bytes.
This way it works as a virtual memory that is then translated to the physical one accordingly to the amount of working bytes. Physical bytes have a life span of more than 100,000 write cycles after which they become unnusable.

## Usage
### **How to include**
```Arduino
#include <Robust_EEPROM.h>

void setup(){

}

void loop(){

}
```

### **Library functions**
#### **`Robust_EEPROM.read( virtual_address )`**

This function allows you to read a single byte of data from the eeprom.
Its only parameter is an `unsigned int` which should be set to the address you wish to read.

The function returns an `unsigned char` containing the value read.

#### **`Robust_EEPROM.write( virtual_address, value )`**

The `write()` method allows you to write a single byte of data to the EEPROM.
Two parameters are needed. The first is an `unsigned int` containing the virtual address that is to be converted
to the physical one to be written, and the second is a the data to be written (`unsigned char`).

This function does not return any value.

#### **`Robust_EEPROM.update( virtual_address, value )`**

This function is similar to `Robust_EEPROM.write()` however this method will only write data if
the cell contents pointed to by the `virtual address` is different to `value`. This method can
help prevent unnecessary wear on the EEPROM cells.

This function does not return any value.

#### **`Robust_EEPROM.fullreset()`**
This functions zeroes the entire physical memory associated to the virtual one.

#### **`Robust_EEPROM.controlLength()`**
This functions returns an `unsigned int` with the amount of control bytes.

#### **`Robust_EEPROM.dataLength()`**
This functions returns an `unsigned int` with the amount of data bytes used as virtual data memory.

#### **`Robust_EEPROM.physicalLength()`**
This functions returns an `unsigned int` with the amount of physical bytes used by the entire virtual memory.

#### **`Robust_EEPROM.netLength()`**
This functions returns an `unsigned int` with the amount of bytes still availlable for the data virtual memory.
As physical memory bytes start to fail this memory decreses in size down to zero.

#### **`Robust_EEPROM.allocatedLength()`**
This functions returns an `unsigned int` with the amount of bytes being used by virtual memory.
As new data is added this value increases up to the total extent of the written data on virtual memory.

#### **`Robust_EEPROM.physicalByte( virtual_address )`**

This method allows you to get the respective `physical address` from a virtual one. With an ever inreasing
failed bytes these addresses become more different from each other.

### **Examples**
#### **Partial Dummy Memory Allocation (Testing Purposes)**
```Arduino
#include <Robust_EEPROM.h>

int test_array_length = 10;
int *starting_data_array;  
int last_net_length = 0;
int data_health = 0;
char BUFFERNUMBER[4];
Dummy_EEPROM *dummy_eeprom;
Robust_EEPROM *robust_eeprom;
enum Info : uint8_t {virtual_addresses, physical_addresses, memory_data, original_data};
enum Test : uint8_t {testing, result, stop};
Test test = testing;
enum Step : uint8_t {test1, test2, test3};
Step step = test1;
bool passed = true;

void setup() {

    Serial.begin(9600);
    Serial.println("");
    Serial.print("Starting...");

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

    for (int i = 0; i < test_array_length; i++)
        robust_eeprom->update(i, rand() % 256);
        
    starting_data_array = new int[test_array_length];
    for (int i = 0; i < test_array_length; i++)
        starting_data_array[i] = robust_eeprom->read(i);

}

void printInfo (Info info) {
    Serial.println("");
    switch (info) {
        case virtual_addresses:
            Serial.println("Virtual Addresses");
            break;
        case physical_addresses:
            Serial.println("Physical Addresses");
            break;
        case memory_data:
            Serial.println("Memory Data (second half unchanged)");
            break;
        case original_data:
            Serial.println("Original Data");
            break;
    }
    Serial.print("\t");
    for (int i = 0; i < test_array_length; i++) {
        switch (info) {
            case virtual_addresses:
                sprintf (BUFFERNUMBER, "%3d", i);
                break;
            case physical_addresses:
                sprintf (BUFFERNUMBER, "%3d", robust_eeprom->physicalByte(i));
                break;
            case memory_data:
                sprintf (BUFFERNUMBER, "%3d", robust_eeprom->read(i));
                break;
            case original_data:
                sprintf (BUFFERNUMBER, "%3d", starting_data_array[i]);
                break;
        }
        Serial.print(BUFFERNUMBER);
        if (i < test_array_length - 1)
            Serial.print(":");
    }
}

void printMemoryStats () {
    Serial.println("");
    Serial.println("(Allocation) of (Net Length) of (Data Length) of (Total Lenght) = (Data Memory Health)");
    Serial.print("\t");
    Serial.print(robust_eeprom->allocatedLength());
    Serial.print(" of ");
    Serial.print(robust_eeprom->netLength());
    Serial.print(" of ");
    Serial.print(robust_eeprom->dataLength());
    Serial.print(" of ");
    Serial.print(robust_eeprom->totalLength());
    Serial.print(" = ");
    Serial.print(data_health);
    Serial.print("%");
}

void printFullInfo () {
    Serial.println("");
    printInfo(virtual_addresses);
    printInfo(physical_addresses);
    printInfo(memory_data);
    printInfo(original_data);
    printMemoryStats();
}

void loop() {

    if (test == testing) {

        if (last_net_length != robust_eeprom->netLength()) {

            data_health = (int)(100*(double)robust_eeprom->netLength()/robust_eeprom->dataLength());

            printFullInfo();

            // Does a full reset at 50% to check reseting memmory
            if (data_health < 25 && step == test1) {
                step = test2;
                robust_eeprom->fullreset();
                for (int i = 0; i < test_array_length; i++)
                    robust_eeprom->update(i, starting_data_array[i]);
                printFullInfo();
                // Check memory integrity
                Serial.println("");
                for (int i = 0; i < test_array_length; i++) {
                    if (starting_data_array[i] != robust_eeprom->read(i)) {
                        passed = false;
                        Serial.print("Test 1: Failed rebuilding original data after full reset! <<!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                        test = result;
                        break;
                    }
                }
                if (passed == true)
                    Serial.print("Test 1: Passed rebuilding original data after full reset! <<✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓");
            }

            // Conclusion tests condition
            if (robust_eeprom->allocatedLength() == robust_eeprom->netLength())
                test = result;

            last_net_length = robust_eeprom->netLength();
        }
        
        for (int i = 0; i < test_array_length / 2; i++) // First half data randomly generated (volatile)
            robust_eeprom->update(i, rand() % 256);
    
    } else if (test == result) {

        // Check memory integrity
        Serial.println("");
        for (int i = test_array_length / 2; i < test_array_length; i++) {
            if (starting_data_array[i] != robust_eeprom->read(i)) {
                Serial.print("Test 2: Failed preservation of unchanged data! <<!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                passed = false;
                break;
            }
        }
        if (passed == true)
            Serial.print("Test 2: Passed preservation of unchanged data! <<✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓✓");
        Serial.println("");
        Serial.println("");
        Serial.println("***************************************************************************************************");
        if (passed)
            Serial.println("ALL TESTS PASSED✓");
        else
            Serial.println("TESTING FAILED!");
                
        test = stop;
    }
}
```