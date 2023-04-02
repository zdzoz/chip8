//
// Created by Oskar Zdziarski on 3/30/23.
//

#ifndef CHIP8_WINDOW_H
#define CHIP8_WINDOW_H

#include <SDL.h>
#include <SDL_ttf.h>

#define TRUE 1
#define FALSE 0

#define EMULATOR_WIDTH 960
#define EMULATOR_HEIGHT 480
#define PIXEL_W (EMULATOR_WIDTH/64)
#define PIXEL_H (EMULATOR_HEIGHT/32)

#ifdef NDEBUG
    #define DEBUG_WINDOW(win)
    #define WINDOW_WIDTH EMULATOR_WIDTH
#else
    #define DEBUG_WINDOW(win) DebugWindow(win)
    #define WINDOW_WIDTH (EMULATOR_WIDTH + 300)
    #define DEBUG_WIDTH (WINDOW_WIDTH - EMULATOR_WIDTH)
    #define DEBUG_HEIGHT EMULATOR_HEIGHT
    #define DEBUG_CLEAR_COLOR 0x33, 0x33, 0x33, 0x33
    static SDL_Rect debugRect = {EMULATOR_WIDTH, 0, DEBUG_WIDTH, DEBUG_HEIGHT};

    #define DebugText(win, text, x, y) RenderText(win, text, EMULATOR_WIDTH + x, y, FALSE, FALSE, TRUE)
    #define DebugTextB(win, text, x, y) RenderText(win, text, EMULATOR_WIDTH + x, y, TRUE, FALSE, TRUE)
    #define DebugTextR(win, text, x, y) RenderText(win, text, EMULATOR_WIDTH + x, y, FALSE, TRUE, TRUE)
    #define DebugTextBR(win, text, x, y) RenderText(win, text, EMULATOR_WIDTH + x, y, TRUE, TRUE, TRUE)
#endif
#define WINDOW_HEIGHT 480

#define PIXEL_ON 0xff, 0xff, 0xff, 0xff
#define PIXEL_OFF 0, 0, 0, 0

typedef struct Window {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
} Window;

int SetupWindow(Window* window, const char *title);
void CleanWindow(Window* window);

void DrawPixel(Window* win, uint8_t x, uint8_t y);

void ClearScreen(Window* window);

#ifndef NDEBUG
void RenderText(Window* win, const char* text, int x, int y, int bot, int right, int debug);
void DebugWindow(Window* window);
#endif

#endif //CHIP8_WINDOW_H
