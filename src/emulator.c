#include <stdio.h>
#include <SDL.h>

#include "emulator.h"

#define bool uint8_t
#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0

#define UNIMPLEMENTED {disassemble(); printf("\tINSTRUCTION UNIMPLEMENTED - %s:%d\n", __FILE_NAME__, __LINE__); exit(-1);}

State state;

static bool isEmulatorRunning;

uint64_t TIMER;

static int KEY;

static byte display[EMULATOR_WIDTH_PX * EMULATOR_HEIGHT_PX];

static inline bool push(int x, int y) {
    byte* px = &display[x + y * EMULATOR_WIDTH_PX];
    if (*px == ON) {
        *px = OFF;
        return TRUE;
    }
    *px = ON;
    return FALSE;
}

void emulator(const unsigned char* program, long programSize, Window* win) {
    SDL_Event e;

    KEY = -1;
    TIMER = 0;

    state.pc = 0x200;
    memcpy(&state.memory[0x200], program, programSize * sizeof(char));

    // load digit sprites into memory
    memcpy(&state.memory[0x50], "\xf0\x90\x90\x90\xf0", 5 * sizeof(char)); // 0
    memcpy(&state.memory[0x55], "\x20\x60\x20\x20\x70", 5 * sizeof(char)); // 1
    memcpy(&state.memory[0x5a], "\xF0\x10\xF0\x80\xF0", 5 * sizeof(char)); // 2
    memcpy(&state.memory[0x5f], "\xF0\x10\xF0\x10\xF0", 5 * sizeof(char)); // 3
    memcpy(&state.memory[0x64], "\x90\x90\xF0\x10\x10", 5 * sizeof(char)); // 4
    memcpy(&state.memory[0x69], "\xF0\x80\xF0\x10\xF0", 5 * sizeof(char)); // 5
    memcpy(&state.memory[0x6e], "\xF0\x80\xF0\x90\xF0", 5 * sizeof(char)); // 6
    memcpy(&state.memory[0x73], "\xF0\x10\x20\x40\x40", 5 * sizeof(char)); // 7
    memcpy(&state.memory[0x78], "\xF0\x90\xF0\x90\xF0", 5 * sizeof(char)); // 8
    memcpy(&state.memory[0x7d], "\xF0\x90\xF0\x10\xF0", 5 * sizeof(char)); // 9
    memcpy(&state.memory[0x82], "\xF0\x90\xF0\x90\x90", 5 * sizeof(char)); // a
    memcpy(&state.memory[0x87], "\xE0\x90\xE0\x90\xE0", 5 * sizeof(char)); // b
    memcpy(&state.memory[0x8c], "\xF0\x80\x80\x80\xF0", 5 * sizeof(char)); // c
    memcpy(&state.memory[0x91], "\xE0\x90\x90\x90\xE0", 5 * sizeof(char)); // d
    memcpy(&state.memory[0x96], "\xF0\x80\xF0\x80\xF0", 5 * sizeof(char)); // e
    memcpy(&state.memory[0x9b], "\xF0\x80\xF0\x80\x80", 5 * sizeof(char)); // f

    uint64_t NOW = 0, LAST;
    float* deltaTime = (float*)&DELTA_TIME;
    uint32_t* fps = (uint32_t*)&FPS;

    isEmulatorRunning = TRUE;
    while ((state.pc - 0x200 < programSize) && isEmulatorRunning) {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        *deltaTime = (NOW - LAST) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
        *fps = 1000.0f / DELTA_TIME;
//        printf("%d\n", FPS);
//        printf("%f\n", DELTA_TIME);
        SDL_Delay(floor(1000.0/60.0 - DELTA_TIME));

        //        state.pc += disassemble();
        state.pc += emulateCPU();

        for (int i = 0; i < 2048; i++) {
            byte pixel = display[i];
            if (pixel == ON) {
                DrawPixel(win, i % EMULATOR_WIDTH_PX, i / EMULATOR_WIDTH_PX);
            }
        }

        DEBUG_WINDOW(win);

        DrawScreen(win);
        ClearScreen(win);

        KEY = -1;
        // Poll window events
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: isEmulatorRunning = FALSE; break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_0: KEY = 0; break;
                        case SDLK_1: KEY = 1; break;
                        case SDLK_2: KEY = 2; break;
                        case SDLK_3: KEY = 3; break;
                        case SDLK_4: KEY = 4; break;
                        case SDLK_5: KEY = 5; break;
                        case SDLK_6: KEY = 6; break;
                        case SDLK_7: KEY = 7; break;
                        case SDLK_8: KEY = 8; break;
                        case SDLK_9: KEY = 9; break;
                        case SDLK_a: KEY = 0xa; break;
                        case SDLK_b: KEY = 0xb; break;
                        case SDLK_c: KEY = 0xc; break;
                        case SDLK_d: KEY = 0xd; break;
                        case SDLK_e: KEY = 0xe; break;
                        case SDLK_f: KEY = 0xf; break;
                        default: KEY = -1;
                    }
                    break;
            }
        }
    }
    DeleteStack(state.sp);
}

uint32_t emulateCPU() {
    uint16_t opcode = state.memory[state.pc] << 8;
    opcode += state.memory[state.pc + 1];
    uint32_t opbyte = 2;

    if (state.DT != 0) {
        state.DT--;
        return 0;
    } else if (TIMER != 0){
        printf("TIME TAKEN: %f\n", (SDL_GetPerformanceCounter() - TIMER) / (float)SDL_GetPerformanceFrequency());
        TIMER = 0;
    }

    switch (opcode >> 12) {
        case 0x0:
            if (opcode == 0x00e0) memset(display, OFF, sizeof(display));  // CLEAR SCREEN (CLS)
            if (opcode == 0x00ee) {  // RETURN (RET)
                state.pc = PopStack(&state.sp);
            }
            break;
        case 0x1: state.pc = (opcode & 0xfff); opbyte = 0; break; // 1nnn - JP addr
        case 0x2: // 2nnn - CALL addr
        {
            PushStack(&state.sp, state.pc);
            state.pc = opcode & 0xfff;
            opbyte = 0;
            break;
        }
        case 0x3: { // 3xkk - SE Vx, byte
            if (state.v[(opcode >> 8) & 0xf] == (opcode & 0xff)) opbyte += 2;
        } break;
        case 0x4: { // 4xkk - SNE Vx, byte
            if (state.v[(opcode >> 8) & 0xf] != (opcode & 0xff)) opbyte += 2;
        } break;
        case 0x5: UNIMPLEMENTED; // 5xy0 - SE Vx, Vy
        case 0x6: state.v[opcode >> 8 & 0xf] = opcode & 0x00ff; break; // 6xkk - LD Vx, byte
        case 0x7: state.v[opcode >> 8 & 0xf] += opcode & 0x00ff; break; // 7xkk - ADD Vx, byte
        case 0x8: UNIMPLEMENTED ;
        case 0x9: UNIMPLEMENTED;
        case 0xa: state.I = (opcode & 0x0fff); break; //  Annn - LD I, addr
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
        case 0xf: {
            uint16_t sub = opcode & 0xff;
            uint8_t x = (opcode >> 8) & 0xf;
            if (sub == 0x0a) { // Fx0A - LD Vx, K
                if (KEY < 0) opbyte = 0;
                else state.v[x] = KEY;
            }
            else if (sub == 0x29) { // Fx29 - LD F, Vx
                state.I = 0x50 + 5 * state.v[x];
            }
            else if (sub == 0x15) { // Fx15 - LD DT, Vx
                state.DT = state.v[x];
                TIMER = SDL_GetPerformanceCounter();
            }
            else if (sub == 0x07) { // Fx07 - LD Vx, DT
                state.v[x] = state.DT;
            }
            else UNIMPLEMENTED;
        }
            break;
    }
    return opbyte;
}

int disassemble() {
    uint16_t opcode = state.memory[state.pc] << 8;
    opcode += state.memory[state.pc + 1];

    printf("(%04x) - ", opcode);
    printf("0x%03x ", state.pc);
    switch ((opcode & 0xf000) >> 12) {
        case 0x0:
            if (opcode == 0x00e0) printf("CLS");
            if (opcode == 0x00ee) printf("RET");
            break;
        case 0x1: printf("JP 0x%03x", opcode & 0xfff); break; // 1nnn - JP addr
        case 0x2: printf("CALL 0x%03x", opcode & 0xfff); break;
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
