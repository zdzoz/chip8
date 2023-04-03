//
// Created by ザスキ・オスカー on 3/31/23.
//

#ifndef CHIP8_EMULATOR_H
#define CHIP8_EMULATOR_H

#include "window.h"

typedef uint8_t byte;

const double DELTA_TIME;
const uint32_t FPS;

typedef struct {
    uint32_t pc;
    uint16_t I;     // I registers
    byte v[16]; //  V registers
    const unsigned char* memory;
} State;

void emulator(const unsigned char *program, long programSize, struct Window *win);

uint32_t emulateCPU(Window* win);

int disassemble();

#endif //CHIP8_EMULATOR_H
