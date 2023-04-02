#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "emulator.h"

void ReadProgram(const char*, const unsigned char**, long*);

int main(int argc, char* args[]) {
    const unsigned char* program = NULL;
    long programSize;
    ReadProgram(args[1], &program, &programSize);

    Window win;
    SetupWindow(&win, "Chip8 Emulator");

    emulator(program, programSize, &win);

    CleanWindow(&win);
    free((void*)program);
    return 0;
}

void ReadProgram(const char* loc, const unsigned char** program, long* programSize) {
    FILE* fp  = fopen(loc, "rb");
    if (fp == NULL) {
        printf("Could not read file\n");
        exit(-1);
    }
    printf("filepath: %s\n", loc);
    fseek(fp, 0L, SEEK_END);
    *programSize = ftell(fp);
    rewind(fp);
    printf("filesize: %ld bytes\n", *programSize);

    *program = malloc(*programSize + 1);
    fread(*program, sizeof(char), *programSize, fp);
    fclose(fp);
}