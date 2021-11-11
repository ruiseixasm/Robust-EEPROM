## **Robust-EEPROM Library V1.0** for Arduino
**Written by:** _Rui Seixas Monteiro_.

## Installation
Create a new folder called "Robust_EEPROM" under the folder named "libraries" in your Arduino sketchbook folder.
Create the folder "libraries" in case it does not exist yet. Place all the files in the "Robust_EEPROM" folder.

## Import
To use the library in your own sketch, select it from *Sketch > Import Library*.

## What is Robust_EEPROM
The Robust_EEPROM library alows the use of unalocated bytes on an EEPROM memory as redundant memory for failed allocated bytes. It does this by using 8/9ths as data bytes and 1/9th as control bytes of the total physical memory,
these control bytes are bit markers of failed data bytes.
This way it works as a virtual memory that is then translated to the physical one accordingly to the amount of
working bytes. Physical bytes have a life span of more than 100,000 write cycles after which they become unnusable.

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

#### **`Robust_EEPROM.controllength()`**
This functions returns an `unsigned int` with the amount of control bytes.

#### **`Robust_EEPROM.datalength()`**
This functions returns an `unsigned int` with the amount of data bytes used as virtual data memory.

#### **`Robust_EEPROM.physicallength()`**
This functions returns an `unsigned int` with the amount of physical bytes used by the entire virtual memory.

#### **`Robust_EEPROM.length()`**
This functions returns an `unsigned int` with the amount of bytes still availlable for the data virtual memory.
As physical memory bytes start to fail this memory decreses in size.

#### **`Robust_EEPROM.physicalbyte( virtual_address )`**

This method allows you to get the respective `physical address` from a virtual one. With an ever inreasing
failed bytes these addresses become more different from each other.

### **Examples**
#### **Full Memory Allocation**
```Arduino
#include "Robust_EEPROM.h"

Dummy_EEPROM *dummy_eeprom;
Robust_EEPROM *robust_eeprom;

void setup() {
    Serial.begin(9600);
    Serial.println("Preparing!");

    dummy_eeprom = new Dummy_EEPROM(1024/4);
    robust_eeprom = new Robust_EEPROM();
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

            data_health = (int)floor(100*robust_eeprom->length()/robust_eeprom->datalength());

            Serial.println("Virtual Adresses");
            for (int i = 0; i < 10; i++) {
                Serial.print(i);
                Serial.print(":");
            }
            Serial.println("Physical Adresses");
            for (int i = 0; i < 10; i++) {
                Serial.print(robust_eeprom->physicalbyte(i));
                Serial.print(":");
            }
            Serial.println("Data");
            for (int i = 0; i < 10; i++) {
                Serial.print(robust_eeprom->read(i));
                Serial.print(":");
            }
            Serial.println("");
            Serial.print(robust_eeprom->length());
            Serial.print(" of ");
            Serial.print(robust_eeprom->datalength());
            Serial.print(" of ");
            Serial.print(robust_eeprom->physicallength());
            Serial.print(" = ");
            Serial.print(data_health);
            Serial.println("%");
            Serial.println("");
            if (data_health < 20) {  // Last Percentage %
                stop = true;
                Serial.println("--------------------------");
                Serial.println(robust_eeprom->length());
                Serial.println(robust_eeprom->datalength());
            }
            
            length = robust_eeprom->length();
        }
        
    }

}
```