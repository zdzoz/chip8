//
// Created by ザスキ・オスカー on 3/30/23.
//

#ifndef CHIP8_WINDOW_H
#define CHIP8_WINDOW_H

#include <SDL.h>
#include <SDL_ttf.h>

#define TRUE 1
#define FALSE 0

#define EMULATOR_WIDTH 960
#define EMULATOR_HEIGHT 480

#ifdef NDEBUG
    #define DEBUG_WINDOW(win)
    #define WINDOW_WIDTH EMULATOR_WIDTH
#else
    #define DEBUG_WINDOW(win, state) DebugWindow(win, state)
    #define WINDOW_WIDTH (EMULATOR_WIDTH + 300)
    #define DEBUG_WIDTH (WINDOW_WIDTH - EMULATOR_WIDTH)
    #define DEBUG_HEIGHT EMULATOR_HEIGHT
    #define DEBUG_CLEAR_COLOR 0x33, 0x33, 0x33, 0x33
    #define RenderTextDB(win, text, x, y) RenderText_(win, text, EMULATOR_WIDTH + x, y, FALSE, FALSE, TRUE)
    #define RenderTextDBBot(win, text, x, y) RenderText_(win, text, EMULATOR_WIDTH + x, y, TRUE, FALSE, TRUE)
    #define RenderTextDBRight(win, text, x, y) RenderText_(win, text, EMULATOR_WIDTH + x, y, FALSE, TRUE, TRUE)
    #define RenderTextDBBotRight(win, text, x, y) RenderText_(win, text, EMULATOR_WIDTH + x, y, TRUE, TRUE, TRUE)
    static SDL_Rect debugRect = {EMULATOR_WIDTH, 0, DEBUG_WIDTH, DEBUG_HEIGHT};
#endif
#define WINDOW_HEIGHT 480

#define CLEAR_COLOR 0x33, 0x33, 0xff, 0xff

typedef struct Window {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
} Window;

int SetupWindow(Window* window, const char *title);
void CleanWindow(Window* window);

void ClearScreen(Window* window);

#define RenderText(win, text, x, y) RenderText_(win, text, x, y, FALSE, FALSE, FALSE);
#define RenderTextBot(win, text, x, y) RenderText_(win, text, x, y, TRUE, FALSE, FALSE);
#define RenderTextRight(win, text, x, y) RenderText_(win, text, x, y, FALSE, TRUE, FALSE);
void RenderText_(Window* win, const char* text, int x, int y, int bot, int right, int debug);

void DebugWindow(Window* window, void* state);

#endif //CHIP8_WINDOW_H
