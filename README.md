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
            if (data_health < 20) {  // Last Percentage %
                stop = true;
                Serial.println("Finish!");
            }
            
            length = robust_eeprom->length();
        }
        
    }

}
```

#### **Partial Memory Allocation**
```Arduino
#include <Robust_EEPROM.h>

int length = 0;
int data_health = 0;
bool stop = false;
Robust_EEPROM *robust_eeprom;

void setup() {
    Serial.begin(9600);
    Serial.println("Preparing!");

    robust_eeprom = new Robust_EEPROM(50, 100); // PARTIAL MEMORY ALLOCATION
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
            if (data_health < 20) {  // Last Percentage %
                stop = true;
                Serial.println("Finish!");
            }
            
            length = robust_eeprom->length();
        }
        
    }

}
```

#### **Full Dummy Memory Allocation**
```Arduino
#include <Robust_EEPROM.h>

int length = 0;
int data_health = 0;
bool stop = false;
Dummy_EEPROM *dummy_eeprom;
Robust_EEPROM *robust_eeprom;

void setup() {
    Serial.begin(9600);
    Serial.println("Preparing!");

    dummy_eeprom = new Dummy_EEPROM(1024/4); // Avoid using all 1024 board RAM memory
    robust_eeprom = new Robust_EEPROM(dummy_eeprom); // FULL DUMMY ALLOCATION
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
            if (data_health < 20) {  // Last Percentage %
                stop = true;
                Serial.println("Finish!");
            }
            
            length = robust_eeprom->length();
        }
        
    }

}
```

#### **Partial Dummy Memory Allocation**
```Arduino
#include <Robust_EEPROM.h>

int length = 0;
int data_health = 0;
bool stop = false;
Dummy_EEPROM *dummy_eeprom;
Robust_EEPROM *robust_eeprom;

void setup() {
    Serial.begin(9600);
    Serial.println("Preparing!");

    dummy_eeprom = new Dummy_EEPROM(1024/4); // Avoid using all 1024 board RAM memory
    robust_eeprom = new Robust_EEPROM(50, 100, dummy_eeprom); // PARTIAL DUMMY ALLOCATION
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
            if (data_health < 20) {  // Last Percentage %
                stop = true;
                Serial.println("Finish!");
            }
            
            length = robust_eeprom->length();
        }
        
    }

}
```

#### **Typical Output**
```Arduino
Preparing!
Starting!       

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses   
    50:51:52:53:54:55:56:57:58:59:
Data
    186:137:172:163:189:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    88 of 88 of 100 = 100%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses       
    50:51:52:54:55:56:57:58:59:60:
Data
    17:66:241:58:224:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    87 of 88 of 100 = 98%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    50:52:54:55:56:57:58:59:60:61:
Data
    33:148:224:81:96:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    86 of 88 of 100 = 97%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    52:54:55:56:57:58:59:60:61:62:
Data
    149:147:60:78:80:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    85 of 88 of 100 = 96%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    52:55:56:57:58:59:60:61:62:63:
Data
    195:59:93:201:253:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    84 of 88 of 100 = 95%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    55:56:57:58:59:60:61:62:63:64:
Data
    8:25:153:203:38:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    83 of 88 of 100 = 94%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    56:57:58:59:60:61:62:63:64:65:
Data
    241:162:100:75:35:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    82 of 88 of 100 = 93%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    56:58:59:60:61:62:63:64:65:66:
Data
    72:32:183:172:230:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    81 of 88 of 100 = 92%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    56:59:60:61:62:63:64:65:66:67:
Data
    18:217:42:99:89:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    80 of 88 of 100 = 90%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    56:60:61:62:63:64:65:66:67:68:
Data
    47:45:95:121:27:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    79 of 88 of 100 = 89%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    60:61:62:63:64:65:66:67:68:69:
Data
    170:41:239:190:101:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    78 of 88 of 100 = 88%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    60:62:63:64:65:66:67:68:69:70:
Data
    5:128:174:60:40:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    77 of 88 of 100 = 87%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    60:63:64:65:66:67:68:69:70:71:
Data
    31:252:91:226:44:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    76 of 88 of 100 = 86%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    63:64:65:66:67:68:69:70:71:72:
Data
    153:12:239:248:187:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    75 of 88 of 100 = 85%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    64:65:66:67:68:69:70:71:72:73:
Data
    59:3:77:114:134:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    74 of 88 of 100 = 84%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    65:66:67:68:69:70:71:72:73:74:
Data
    143:214:9:207:90:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    73 of 88 of 100 = 82%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    66:67:68:69:70:71:72:73:74:75:
Data
    1:241:13:203:6:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    72 of 88 of 100 = 81%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    66:68:69:70:71:72:73:74:75:76:
Data
    224:242:62:149:11:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    71 of 88 of 100 = 80%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    66:69:70:71:72:73:74:75:76:77:
Data
    237:158:170:194:16:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    70 of 88 of 100 = 79%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    69:70:71:72:73:74:75:76:77:78:
Data
    195:91:5:239:38:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    69 of 88 of 100 = 78%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    70:71:72:73:74:75:76:77:78:79:
Data
    62:211:106:28:153:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    68 of 88 of 100 = 77%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    71:72:73:74:75:76:77:78:79:80:
Data
    90:33:154:130:198:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    67 of 88 of 100 = 76%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    71:73:74:75:76:77:78:79:80:81:
Data
    184:144:89:192:113:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    66 of 88 of 100 = 75%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    71:74:75:76:77:78:79:80:81:82:
Data
    22:163:237:250:159:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    65 of 88 of 100 = 73%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    74:75:76:77:78:79:80:81:82:83:
Data
    60:105:138:102:153:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    64 of 88 of 100 = 72%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    75:76:77:78:79:80:81:82:83:84:
Data
    24:241:201:108:137:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    63 of 88 of 100 = 71%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    75:77:78:79:80:81:82:83:84:85:
Data
    201:238:27:21:186:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    62 of 88 of 100 = 70%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    75:78:79:80:81:82:83:84:85:86:
Data
    109:40:28:83:60:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    61 of 88 of 100 = 69%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    78:79:80:81:82:83:84:85:86:87:
Data
    207:144:231:175:197:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    60 of 88 of 100 = 68%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    79:80:81:82:83:84:85:86:87:88:
Data
    152:78:123:152:232:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    59 of 88 of 100 = 67%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    79:81:82:83:84:85:86:87:88:89:
Data
    152:15:134:169:190:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    58 of 88 of 100 = 65%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    81:82:83:84:85:86:87:88:89:90:
Data
    91:36:187:6:248:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    57 of 88 of 100 = 64%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    82:83:84:85:86:87:88:89:90:91:
Data
    72:218:30:39:90:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    56 of 88 of 100 = 63%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    83:84:85:86:87:88:89:90:91:92:
Data
    164:56:175:186:66:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    55 of 88 of 100 = 62%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    84:85:86:87:88:89:90:91:92:93:
Data
    217:190:103:213:146:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    54 of 88 of 100 = 61%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    85:86:87:88:89:90:91:92:93:94:
Data
    24:175:7:169:57:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    53 of 88 of 100 = 60%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    86:87:88:89:90:91:92:93:94:95:
Data
    117:218:114:249:0:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    52 of 88 of 100 = 59%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    87:88:89:90:91:92:93:94:95:96:
Data
    202:10:132:146:104:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    51 of 88 of 100 = 57%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    88:89:90:91:92:93:94:95:96:97:
Data
    64:32:117:106:246:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    50 of 88 of 100 = 56%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    89:90:91:92:93:94:95:96:97:98:
Data
    118:64:177:38:34:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    49 of 88 of 100 = 55%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    89:91:92:93:94:95:96:97:98:99:
Data
    151:224:108:96:34:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    48 of 88 of 100 = 54%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    89:92:93:94:95:96:97:98:99:100:
Data
    152:72:240:241:87:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    47 of 88 of 100 = 53%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    89:92:94:95:96:97:98:99:100:101:
Data
    54:102:138:141:109:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    46 of 88 of 100 = 52%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    92:94:95:96:97:98:99:100:101:102:
Data
    103:82:51:177:195:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    45 of 88 of 100 = 51%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    94:95:96:97:98:99:100:101:102:103:
Data
    149:187:90:90:157:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    44 of 88 of 100 = 50%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    94:96:97:98:99:100:101:102:103:104:
Data
    116:238:53:217:130:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    43 of 88 of 100 = 48%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    96:97:98:99:100:101:102:103:104:105:
Data
    165:247:123:96:15:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    42 of 88 of 100 = 47%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    96:98:99:100:101:102:103:104:105:106:
Data
    239:161:118:14:238:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    41 of 88 of 100 = 46%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    98:99:100:101:102:103:104:105:106:107:
    Data
    178:154:161:220:207:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    40 of 88 of 100 = 45%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    99:100:101:102:103:104:105:106:107:108:
Data
    34:108:247:2:70:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    39 of 88 of 100 = 44%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    99:101:102:103:104:105:106:107:108:109:
Data
    193:110:165:153:10:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    38 of 88 of 100 = 43%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    101:102:103:104:105:106:107:108:109:110:
Data
    24:113:247:141:214:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    37 of 88 of 100 = 42%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    101:103:104:105:106:107:108:109:110:111:
Data
    13:217:81:161:188:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    36 of 88 of 100 = 40%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    103:104:105:106:107:108:109:110:111:112:
Data
    207:75:70:115:129:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    35 of 88 of 100 = 39%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    104:105:106:107:108:109:110:111:112:113:
Data
    190:53:210:62:101:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    34 of 88 of 100 = 38%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    105:106:107:108:109:110:111:112:113:114:
Data
    125:249:197:54:124:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    33 of 88 of 100 = 37%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    106:107:108:109:110:111:112:113:114:115:
Data
    157:78:205:82:82:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    32 of 88 of 100 = 36%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    106:107:109:110:111:112:113:114:115:116:
Data
    232:112:95:223:205:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    31 of 88 of 100 = 35%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    107:109:110:111:112:113:114:115:116:117:
Data
    42:146:43:126:86:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    30 of 88 of 100 = 34%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    109:110:111:112:113:114:115:116:117:118:
Data
    168:189:157:100:143:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    29 of 88 of 100 = 32%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    110:111:112:113:114:115:116:117:118:119:
Data
    30:250:18:85:224:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    28 of 88 of 100 = 31%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    110:111:112:114:115:116:117:118:119:120:
Data
    219:170:163:115:181:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    27 of 88 of 100 = 30%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    111:112:114:115:116:117:118:119:120:121:
Data
    222:66:183:174:154:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    26 of 88 of 100 = 29%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    112:114:115:116:117:118:119:120:121:122:
Data
    180:172:42:97:141:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    25 of 88 of 100 = 28%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    114:115:116:117:118:119:120:121:122:123:
Data
    192:14:65:250:130:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    24 of 88 of 100 = 27%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    115:116:117:118:119:120:121:122:123:124:
Data
    136:155:186:59:184:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    23 of 88 of 100 = 26%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    116:117:118:119:120:121:122:123:124:125:
Data
    184:170:141:62:50:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    22 of 88 of 100 = 25%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    117:118:119:120:121:122:123:124:125:126:
Data
    129:105:98:172:49:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    21 of 88 of 100 = 23%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    117:119:120:121:122:123:124:125:126:127:
Data
    225:191:199:112:254:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    20 of 88 of 100 = 22%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    119:120:121:122:123:124:125:126:127:128:
Data
    65:30:16:142:80:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    19 of 88 of 100 = 21%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    119:121:122:123:124:125:126:127:128:129:
Data
    136:4:96:6:49:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    18 of 88 of 100 = 20%

Virtual Adresses
    0:1:2:3:4:5:6:7:8:9:
Physical Adresses
    121:122:123:124:125:126:127:128:129:130:
Data
    195:235:13:214:69:148:140:109:8:154:
length of datalength of physicallenght = data memory health
    17 of 88 of 100 = 19%

Finish!
```
