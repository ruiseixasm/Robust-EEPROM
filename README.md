## **Robust-EEPROM Library V1.0** for Arduino
**Written by:** _Rui Seixas Monteiro_.

## Installation
Create a new folder called "Robust_EEPROM" under the folder named "libraries" in your Arduino sketchbook folder.
Create the folder "libraries" in case it does not exist yet. Place all the files in the "Robust_EEPROM" folder.

## Import
To use the library in your own sketch, select it from *Sketch > Import Library*.

## What is Robust_EEPROM
The Robust_EEPROM library alows the use unalocated bytes on an EEPROM memory as redundant memory for failed bytes
of the alocated one. It does this by using a 1/9th of the total memory as markers bits of failed bytes.

## Usage
### **How to include**
```Arduino
#include <Robust_EEPROM.h>

void setup(){

}

void loop(){

}```
### **Library functions**
