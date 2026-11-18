#pragma once

#include <stdint.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t

#define BYTE 1
#define WORD 2
#define QUAD 4

#define ROM_START 0x0
#define ROM_SIZE 0x00010000 // 65 KiB

#define RAM_START 0x20000000
#define RAM_SIZE 0x00010000 // 64 KiB
