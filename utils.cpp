/**
  @file utils.cpp

  @brief Util functions implementation

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

#include "utils.h"

String stringHexByte(byte value) {
  String retval = "0x";
  if (value < 0x10) retval += "0";
  return retval + String(value, HEX);
}

String stringHexWord(word value) {
  String retval = "0x";
  if (value < 0x10) retval += "0";
  if (value < 0x100) retval += "0";
  if (value < 0x1000) retval += "0";
  return retval + String(value, HEX);
}

long parseHexString() {
  while (0 == Serial.available());

  String input = Serial.readStringUntil('\n');
  input.trim();

  Serial.println(input);

  if (0 == input.length()) return ParsedHexResult::PASS;
  if (!(input.startsWith("0x") || input.startsWith("0X"))) {
    Serial.println("Error: invalid value. Expected integer in HEX format: 0x...");
    return ParsedHexResult::INVALID;
  }

  char *end;
  long val = strtol(input.c_str(), &end, 16);

  if ('\0' != *end) {
    Serial.println("Error: invalid value. Expected integer in HEX format: 0x...");
    return ParsedHexResult::INVALID;
  }

  return val;
}

long parseIntString() {
  while (0 == Serial.available());

  String input = Serial.readStringUntil('\n');
  input.trim();

  Serial.println(input);

  if (0 == input.length()) return ParsedHexResult::PASS;

  char *end;
  long val = strtol(input.c_str(), &end, 10);

  if ('\0' != *end) {
    Serial.println("Error: invalid integer value.");
    return ParsedHexResult::INVALID;
  }

  return val;
}

/**
  Print content of the byte array in one line in hex format with leading zeros if necessary.
  [ AA BB CC DD ... ]
*/
void printBytesHex(byte *buf, int len) {
  Serial.print("[ ");
  for (int i = 0; i < len; i++) {
    byte val = buf[i];
    if (val <= 0xF) Serial.print("0");
    Serial.print(val, HEX);
    Serial.print(" ");
  }
  Serial.println("]");
}
