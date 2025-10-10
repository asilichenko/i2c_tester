/**
  @file i2c_tester.ino

  Arduino sketch that reads commands from the serial port
  and executes corresponding functions over the I²C interface.

  Supported commands:
  + scan – checks for connected devices.
  + connect – attempts to establish a connection with a device.
  + write byte – sends one byte to a device.
  + read bytes - requests a specified number of bytes from the device.

  Arduino Nano standard I2C connection:
  - GND -> black
  - A4 = SDA -> white
  - A5 = SCL -> yellow


  MIT License

  Copyright (c) 2025 Oleksii Sylichenko

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <Wire.h>
#include "utils.h"

// Commands
const String SCAN = "scan";
const String CONNECT = "connect";
const String WRITE_BYTE = "write byte";
const String READ_BYTES = "read bytes";

const int MAX_BYTES_READ = 36;

int connectedAddr = 0;

void setup() {
  Wire.begin();

  Serial.begin(9600);
  while (!Serial);

  Serial.println();
  Serial.println("===============================================");
  Serial.println("I2C Tester");
  Serial.println("- scan");
  Serial.println("- connect");
  Serial.println("- write byte");
  Serial.println("- read bytes");
  Serial.println("===============================================");
  Serial.print("Input command: ");
}

void loop() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  Serial.println("`" + cmd + "`");

  if (SCAN == cmd) {
    scanDevices();
  } else if (CONNECT == cmd) {
    connectDevice();
  } else if (WRITE_BYTE == cmd) {
    writeByte();
  } else if (READ_BYTES == cmd) {
    readBytes();
  } else if (cmd.length() > 0) {
    Serial.println("Unknown command");
  }

  Serial.print("\nInput command: ");
}

void scanDevices() {
  int nDevices = 0;

  Serial.println("Scanning...");

  for (byte address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    /*
      0: success.
      1: data too long to fit in transmit buffer.
      2: received NACK on transmit of address.
      3: received NACK on transmit of data.
      4: other error.
      5: timeout
      https://docs.arduino.cc/language-reference/en/functions/communication/wire/endTransmission/
    */

    if (0 == error) {
      Serial.println("I2C device found at address: " + stringHexByte(address));
      ++nDevices;
    } else if (4 == error) {
      Serial.println("Unknown error at address: " + stringHexByte(address));
    }
  }
  if (0 == nDevices) {
    Serial.println("No I2C devices found.");
  } else {
    Serial.println("Done.");
  }
}

void connectDevice() {
  byte addr = 0;
  while (true) {
    Serial.print("Input address, or press Enter (leave empty) to exit: ");
    long value = parseHexString();

    if (ParsedHexResult::PASS == value) return;
    if (value < 0) continue;
    if (0 == value || value > 126) {
      Serial.println("Error: address is not in range 0x01 to 0xFE");
    } else {
      addr = value;
      break;
    }
  }

  connectedAddr = 0;  // clear connected address

  Serial.println("Connecting to: " + stringHexByte(addr));

  Wire.beginTransmission(addr);
  byte error = Wire.endTransmission();

  if (0 == error) {
    Serial.println("Connection successful.");
    connectedAddr = addr;
  } else {
    Serial.println("Connection failed. Error code: " + String(error));
  }
}

bool ensureConnected() {
  if (0 == connectedAddr) connectDevice();
  return (connectedAddr > 0);
}

void writeByte() {
  if (!ensureConnected()) return;

  byte byteValue = 0;
  while (true) {
    Serial.print("Input byte value to write, or press Enter (leave empty) to exit: ");
    long value = parseHexString();

    if (ParsedHexResult::PASS == value) return;
    if (value < 0) continue;
    if (value > 0xFF) {
      Serial.println("Error: value is not in range 0x00 to 0xFF");
    } else {
      byteValue = value & 0xFF;
      break;
    }
  }

  Serial.println("Writing byte " + stringHexByte(byteValue) + " to " + stringHexByte(connectedAddr));

  Wire.beginTransmission(connectedAddr);
  Wire.write(byteValue);
  byte error = Wire.endTransmission();
  if (0 == error) Serial.println("ok");
  else Serial.println("Error: " + String(error));
}

void readBytes() {
  if (!ensureConnected()) return;

  int numBytes = 0;
  while (true) {
    Serial.print("Input number of bytes to read, press Enter (leave empty) for 1: ");
    long value = parseIntString();

    if (ParsedHexResult::PASS == value) {
      numBytes = 1;
      break;
    }
    if (value < 0) continue;
    if (value > MAX_BYTES_READ) {
      Serial.println("Error: maximum number to read is " + String(MAX_BYTES_READ));
    } else {
      numBytes = value & 0xFF;
      break;
    }
  }

  Serial.println("Reading " + String(numBytes) + " byte(s) from 0x" + String(connectedAddr, HEX) + ":");

  byte buf[numBytes];
  memset(buf, 0, sizeof(buf));

  bool flag;
  int actual = 0;
  do {
    flag = false;
    Wire.requestFrom(connectedAddr, numBytes - actual);
    while (Wire.available() && actual < numBytes) {
      flag = true;
      buf[actual++] = Wire.read();
    }
  } while (flag && actual < numBytes);

  printBytesHex(buf, actual);
  Serial.println("Actual read: " + String(actual));
}
