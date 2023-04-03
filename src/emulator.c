#include <stdio.h>
#include <SDL.h>

#include "emulator.h"
#include <unistd.h>

#define bool uint8_t
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0

#define UNIMPLEMENTED {disassemble(); printf("\tINSTRUCTION UNIMPLEMENTED - %s:%d\n", __FILE_NAME__, __LINE__); exit(-1);}

State state;

static byte display[EMULATOR_WIDTH_PX * EMULATOR_WIDTH_PX];

static inline bool push(int x, int y) {
    byte* px = &display[x + y * EMULATOR_WIDTH_PX];
    if (*px == ON) {
        *px = OFF;
        return TRUE;
    }
    *px = ON;
}

void emulator(const unsigned char* program, long programSize, Window* win) {
    SDL_Event e;

    state.pc = 0;
    state.memory = program;


    uint64_t NOW = 0, LAST;
    double* deltaTime = (double*)&DELTA_TIME;
    uint32_t* fps = (uint32_t*)&FPS;

    bool isEmulatorRunning = TRUE;
    while ((state.pc < programSize) && isEmulatorRunning) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        *deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency() * 1000);
        SDL_Delay(floor(1000/60.0 - DELTA_TIME));
        *fps = 1000 / *deltaTime;

//        state.pc += disassemble();
        state.pc += emulateCPU(win);

        for (int i = 0; i < 2048; i++) {
            byte pixel = display[i];
            if (pixel == ON) {
                DrawPixel(win, i % EMULATOR_WIDTH_PX, i / EMULATOR_WIDTH_PX);
            }
        }

        DEBUG_WINDOW(win);

        DrawScreen(win);
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
            if (opcode == 0x00e0) printf("CLS\n"); // TODO: CLEAR SCREEN (CLS)
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
            bool collision = FALSE;
            for (byte i = 0; i < n; i++) {
                if((*sprite & 0x80) == 0x80) collision = push((x + 0) % EMULATOR_WIDTH_PX, y % EMULATOR_HEIGHT_PX);
                if((*sprite & 0x40) == 0x40) collision = push((x + 1) % EMULATOR_WIDTH_PX, y % EMULATOR_HEIGHT_PX);
                if((*sprite & 0x20) == 0x20) collision = push((x + 2) % EMULATOR_WIDTH_PX, y % EMULATOR_HEIGHT_PX);
                if((*sprite & 0x10) == 0x10) collision = push((x + 3) % EMULATOR_WIDTH_PX, y % EMULATOR_HEIGHT_PX);
                if((*sprite & 0x08) == 0x08) collision = push((x + 4) % EMULATOR_WIDTH_PX, y % EMULATOR_HEIGHT_PX);
                if((*sprite & 0x04) == 0x04) collision = push((x + 5) % EMULATOR_WIDTH_PX, y % EMULATOR_HEIGHT_PX);
                if((*sprite & 0x02) == 0x02) collision = push((x + 6) % EMULATOR_WIDTH_PX, y % EMULATOR_HEIGHT_PX);
                if((*sprite & 0x01) == 0x01) collision = push((x + 7) % EMULATOR_WIDTH_PX, y % EMULATOR_HEIGHT_PX);
                sprite++;
                y++;
            }
            state.v[0xf] = collision;
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



