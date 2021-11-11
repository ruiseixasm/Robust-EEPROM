## **Robust-EEPROM Library V1.0** for Arduino
**Written by:** _Rui Seixas Monteiro_.

## Installation
Create a new folder called "Robust_EEPROM" under the folder named "libraries" in your Arduino sketchbook folder.
Create the folder "libraries" in case it does not exist yet. Place all the files in the "Robust_EEPROM" folder.

## Import
To use the library in your own sketch, select it from *Sketch > Import Library*.

## What is Robust_EEPROM
The Robust_EEPROM library alows the use unalocated bytes on an EEPROM memory as redundant memory for failed bytes
of the alocated one. It does this by using 8/9ths as data bytes and 1/9th as control bytes of the total memory allocated, these control bytes are bit markers of failed data bytes.
This way it works as a virtual memory that is then translated to the physical one accordingly to the amount of
working bytes.

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
#### **`Robust_EEPROM.read( address )`**

This function allows you to read a single byte of data from the eeprom.
Its only parameter is an `int` which should be set to the address you wish to read.

The function returns an `unsigned char` containing the value read.

#### **`Robust_EEPROM.write( address, value )`**

The `write()` method allows you to write a single byte of data to the EEPROM.
Two parameters are needed. The first is an `int` containing the virtual address that is to be converted to the physical one to be written, and the second is a the data to be written (`unsigned char`).

This function does not return any value.

#### **`Robust_EEPROM.update( address, value )`**

This function is similar to `Robust_EEPROM.write()` however this method will only write data if the cell contents pointed to by the `virtual address` is different to `value`. This method can help prevent unnecessary wear on the EEPROM cells.

This function does not return any value.

#### **`Robust_EEPROM.fullreset()`**
This functions zeroes the entire physical memory associated to.