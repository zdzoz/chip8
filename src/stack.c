//
// Created by Oskar Zdziarski on 5/17/23.
//

#include "stack.h"

uint32_t PopStack(Stack** st) {
    uint32_t ret = (**st).addr;
    Stack* temp = *st;
    (*st) = (*st)->prev;
    free(temp);
    return ret;
}

void PushStack(Stack** st, uint16_t addr) {
    Stack* temp = *st;
    *st = malloc(sizeof(Stack));
    (*st)->addr = addr;
    (*st)->prev = temp;
}

void DeleteStack(Stack* st) {
    while (st != NULL) {
        Stack* temp = st->prev;
        free(st);
        st = temp;
    }
}