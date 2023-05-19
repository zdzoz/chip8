//
// Created by Oskar Zdziarski on 5/17/23.
//

#ifndef CHIP8_STACK_H
#define CHIP8_STACK_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Stack {
    uint32_t addr;
    struct Stack* prev;
} Stack;

uint32_t PopStack(Stack** st);

void PushStack(Stack** st, uint16_t addr);

void DeleteStack(Stack* st);

#endif //CHIP8_STACK_H
