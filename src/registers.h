#pragma once
#include <cstdint>

// Registers
struct RegistersStruct
{
  uint8_t register1;
  uint8_t register2;
  uint8_t register3;
  uint8_t register4;
};

union Registers
{
  uint32_t asInt;
  RegistersStruct asStruct;
};