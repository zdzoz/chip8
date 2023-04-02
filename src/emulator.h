//
// Created by ザスキ・オスカー on 3/31/23.
//

#ifndef CHIP8_EMULATOR_H
#define CHIP8_EMULATOR_H

#include "window.h"

const double DELTA_TIME;

typedef struct {
    uint32_t pc;
    uint16_t I;     // I register
    uint8_t v[16]; //  V registers
    const unsigned char* memory;
} State;

void emulator(const unsigned char *program, long programSize, struct Window *win);

uint32_t emulateCPU(State* state);

int disassemble(State* state);

#endif //CHIP8_EMULATOR_H
