#include <stdio.h>
#include <SDL.h>

#include "emulator.h"
#include <unistd.h>

#define bool char
#define TRUE 1
#define FALSE 0

#define UNIMPLEMENTED {disassemble(); printf("\tINSTRUCTION UNIMPLEMENTED - %s:%d\n", __FILE_NAME__, __LINE__); exit(-1);}

State state;

typedef uint8_t byte;

typedef struct vec2 {
    byte x, y;
} vec2;

static byte* display[2048 >> 3];

static struct Stack {
    vec2 pixels[2048];
    union {
        int size;
        int top;
    };
} stack;

static inline void push(int x, int y) {
    stack.pixels[stack.top++] = (vec2){x, y};
}

static inline void pop() { stack.top--; }

void emulator(const unsigned char* program, long programSize, Window* win) {
    SDL_Event e;
    stack.top = 0;

    state.pc = 0;
    state.memory = program;

    uint64_t NOW = 0, LAST;
    double* deltaTime = (double*)&DELTA_TIME;
    bool isEmulatorRunning = TRUE;
    while ((state.pc < programSize) && isEmulatorRunning) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();

        *deltaTime = (double)((NOW - LAST)*1000 / (double)SDL_GetPerformanceFrequency() );

//        state.pc += disassemble();
        state.pc += emulateCPU(win);

        for (int i = 0; i < stack.size; i++) {
            vec2* s = &stack.pixels[i];
            DrawPixel(win, s->x, s->y);
        }

        DEBUG_WINDOW(win);
        ClearScreen(win);
        // Poll window events
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                isEmulatorRunning = FALSE;
            }
        }
    }
}

uint32_t emulateCPU(Window* win) {
    uint16_t opcode = state.memory[state.pc] << 8;
    opcode += state.memory[state.pc + 1];
    uint32_t opbyte = 2;

    switch ((opcode & 0xf000) >> 12) {
        case 0x0:
            if (opcode == 0x00e0) stack.top = 0;
            if (opcode == 0x00ee) UNIMPLEMENTED; // TODO: RETURN (RET)
            break;
        case 0x1: state.pc = (opcode & 0x0fff) - 0x200; opbyte = 0; break; // 1nnn - JP addr
        case 0x2: UNIMPLEMENTED; // 2nnn - CALL addr
        case 0x3: UNIMPLEMENTED; // 3xkk - SE Vx, byte
        case 0x4: UNIMPLEMENTED; // 4xkk - SNE Vx, byte
        case 0x5: UNIMPLEMENTED; // 5xy0 - SE Vx, Vy
        case 0x6: state.v[opcode >> 8 & 0x0f] = opcode & 0x00ff; break; // 6xkk - LD Vx, byte
        case 0x7: state.v[opcode >> 8 & 0x0f] += opcode & 0x00ff; break; // 7xkk - ADD Vx, byte
        case 0x8: UNIMPLEMENTED;
        case 0x9: UNIMPLEMENTED;
        case 0xa: state.I = (opcode & 0x0fff) - 0x200; break; //  Annn - LD I, addr
        case 0xb: UNIMPLEMENTED;
        case 0xc: UNIMPLEMENTED;
        case 0xd: // Dxyn - DRW Vx, Vy, nibble
        {   // REGISTERS: x, y
            // n = size of sprite
            // I = location of sprite
            byte x = state.v[opcode >> 8 & 0xf];
            byte y = state.v[opcode >> 4 & 0xf];
            byte n = opcode & 0xf;
            const byte* sprite = &state.memory[state.I];
            for (byte i = 0; i < n; i++) {
                const byte b = state.memory[state.I + i];
                byte temp = x;
                if((b & 0x80) == 128) push(temp, y);
                temp++;
                if((b & 0x40) == 64)  push(temp, y);
                temp++;
                if((b & 0x20) == 32)  push(temp, y);
                temp++;
                if((b & 0x10) == 16)  push(temp, y);
                temp++;
                if((b & 0x08) == 8)   push(temp, y);
                temp++;
                if((b & 0x04) == 4)   push(temp, y);
                temp++;
                if((b & 0x02) == 2)   push(temp, y);
                temp++;
                if((b & 0x01) == 1)   push(temp, y);
                y += 1;
            }
        }
        break;
        case 0xe: UNIMPLEMENTED;
        case 0xf: UNIMPLEMENTED;
    }
    return opbyte;
}

int disassemble() {
    uint16_t opcode = state.memory[state.pc] << 8;
    opcode += state.memory[state.pc + 1];

    printf("(%04x) - ", opcode);
    printf("0x%03x ", state.pc + 0x200);
    switch ((opcode & 0xf000) >> 12) {
        case 0x0:
            if (opcode == 0x00e0) printf("CLS");
            if (opcode == 0x00ee) printf("RET");
            break;
        case 0x1: printf("JP 0x%03x", opcode & 0x0fff); break; // 1nnn - JP addr
        case 0x2: break;
        case 0x3: break;
        case 0x4: break;
        case 0x5: break;
        case 0x6: printf("LD V%x, 0x%02x", opcode >> 8 & 0x0f, opcode & 0x00ff); break;
        case 0x7: printf("ADD V%x, 0x%02x", opcode >> 8 & 0x0f, opcode & 0x00ff); break;
        case 0x8: break;
        case 0x9: break;
        case 0xa: printf("LD I, 0x%03x", (opcode & 0x0fff)); break;
        case 0xb: break;
        case 0xc: break;
        case 0xd: printf("DRW V%x, V%x, 0x%x", opcode >> 8 & 0x0f, opcode >> 4 & 0x00f, opcode & 0x000f); break; // Dxyn - DRW Vx, Vy, nibble
        case 0xe: break;
        case 0xf: break;
    }
    printf("\n");
    return 2;
}



