# Arduino I2C Tester
Scans for I2C devices and performs read/write operations based on commands from the Serial Monitor.

## Arduino Nano standard I2C connection
- ⚪ White: A4 - SDA
- 🟡 Yellow: A5 - SCL
- ⚫ Black: GND

## Supported commands
+ `scan` - checks for connected devices.
+ `connect` - checks if a device is available at the specified address. If it is, the address is saved and used for further data exchange.
+ `write byte` - sends one byte to a device.
+ `read bytes` - requests a specified number of bytes from the device.

# Common Use Cases

## Case 1. Scan and Connect

1. Open Serial Monitor:
```
===============================================
I2C Tester
- scan
- connect
- write byte
- read bytes
===============================================
Input command: 
```
2. Enter `scan`:
```
Input command: `scan`
Scanning...
I2C device found at address: 0x55
Done.
```
3. Enter `connect`:
```
Input command: `connect`
Input address, or press Enter (leave empty) to exit: 
```
4. Enter `0x55`:
```
Input command: `connect`
Input address, or press Enter (leave empty) to exit: 0x55
Connecting to: 0x55
Connection successful.
```

Executing the `connect` command is optional - if no address has been saved, the program will automatically prompt you to enter one before any read or write operation.

## Case 2. Write byte and Read bytes

1. Enter `write byte`
2. Enter `0x08` - for example, the _Voltage()_ command of the BQ28Z610 Gas Gauge Device:
```
Input command: `write byte`
Input byte value to write, or press Enter (leave empty) to exit: 0x08
Writing byte 0x08 to 0x55
ok
```
3. Enter `read bytes`:
```
Input command: `read bytes`
Input number of bytes to read, press Enter (leave empty) for 1:
```
4. Enter `2` to read a word:
```
Input command: `read bytes`
Input number of bytes to read, press Enter (leave empty) for 1: 2
Reading 2 byte(s) from 0x55:
[ C9 1A ]
Actual read: 2
```
5. `[ C9 1A ]` = 0x1AC9 (Little Endian) = 6857 (mV)

## Case 3. Working with Registers

As an example, here is the sequence for reading the Chemical ID of the BQ28Z610 device:

<img width="600" height="281" alt="Example: Read Chemical ID()" src="https://github.com/user-attachments/assets/d5f3c47d-0a6c-4082-b055-2aa334bf4350" />

1. `write byte`
2. `0x3e` or `0x00`
```
Input command: `write byte`
Input byte value to write, or press Enter (leave empty) to exit: 0x00
Writing byte 0x00 to 0x55
ok
```
3. `write byte`
4. `0x06`
```
Input command: `write byte`
Input byte value to write, or press Enter (leave empty) to exit: 0x06
Writing byte 0x06 to 0x55
ok
```
5. `write byte`
6. `0x00`
```
Input command: `write byte`
Input byte value to write, or press Enter (leave empty) to exit: 0x00
Writing byte 0x00 to 0x55
ok
```
7. `write byte`
8. `0x3e`
```
Input command: `write byte`
Input byte value to write, or press Enter (leave empty) to exit: 0x3e
Writing byte 0x3e to 0x55
ok
```
9. `read bytes`
10. `36`
```
Input command: `read bytes`
Input number of bytes to read, press Enter (leave empty) for 1: 36
Reading 36 byte(s) from 0x55:
[ 06 00 52 13 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 94 06 ]
Actual read: 36
```

Where:
- `06 00` - Input Command 0x0006 (for verification)
- `52 13` = ChemID 1352
- last two bytes `94 06`:
  - **0x94** - checksum: 0x94 =  0xFF - (0x06 + 0x00 + 0x52 + 0x13)
  - **0x06** - length: two bytes for command, two bytes for result, two bytes for checksum and length

# Sources
- [i2c_tester.ino](i2c_tester.ino) - main Arduino sketch
- [utils.h](utils.h) - util functions header
- [utils.cpp](utils.cpp) - util function implementation

# See Also
- [Custom Driver for Gas Gauging Device BQ28Z610](https://github.com/asilichenko/bq28z610-arduino-driver) - A driver for Arduino that provides functions to interface with the BQ28Z610 battery gas gauge device over I2C protocol.
- [JBL XTREME 2 BMS](https://github.com/asilichenko/jbl-xtreme2-bms) - Arduino sketches to analyse and emulate BMS microchip of the Battery Pack for the JBL XTREME 2 Bluetooth Speaker.
