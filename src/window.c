#include "window.h"

#include "emulator.h"

extern State state;

static SDL_Rect pixelrect;

int SetupWindow(Window* win, const char *title) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize the SDL2: %s\n", SDL_GetError());
        exit(-1);
    }

    if (TTF_Init() < 0) {
        printf("Failed to initialize SDL2_ttf: %s", TTF_GetError());
        exit(-1);
    }

    win->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (win->window == NULL) {
        printf("Failed to create SDL2 Window\n");
        exit(-1);
    }

    win->renderer = SDL_CreateRenderer(win->window,-1, SDL_RENDERER_ACCELERATED);
    if (win->renderer == NULL) {
        printf("Failed to create renderer: %s", SDL_GetError());
        exit(-1);
    }

    win->font = TTF_OpenFont("fonts/Cozette/CozetteVector.ttf", 18);
    if (win->font == NULL) {
        printf("Failed to read font: %s\n", TTF_GetError());
        exit(-1);
    }

    pixelrect.w = PIXEL_W;
    pixelrect.h = PIXEL_H;
    return TRUE;
}

void CleanWindow(Window* win) {
    TTF_CloseFont(win->font);
    TTF_Quit();

    SDL_DestroyRenderer(win->renderer);
    SDL_DestroyWindow(win->window);
    SDL_Quit();
}

void DrawPixel(Window* win, const uint8_t x, const uint8_t y) {
    SDL_SetRenderDrawColor(win->renderer, PIXEL_ON);
    pixelrect.x = x * PIXEL_W;
    pixelrect.y = y * PIXEL_H;
    SDL_RenderFillRect(win->renderer, &pixelrect);
}

void DrawScreen(Window* win) {
    SDL_RenderPresent(win->renderer);
}

void ClearScreen(Window* win) {
    SDL_SetRenderDrawColor(win->renderer, PIXEL_OFF);
    SDL_RenderClear(win->renderer);
}


// DEBUG FUNCTIONS
#ifndef NDEBUG
void RenderText(Window* win, const char* text, int x, int y, int bot, int right, int debug) {
    SDL_Surface* surface = TTF_RenderText_Solid(win->font, text, (SDL_Color) { 0xff, 0xff, 0xff, 0xff });
    // convert surface to texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(win->renderer, surface);

    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

    if (bot == TRUE) y += WINDOW_HEIGHT - texH;
    if (right == TRUE) {
        x += debug == TRUE ? DEBUG_WIDTH - texW : WINDOW_WIDTH - texW;
    }

    if (x > WINDOW_WIDTH || x < 0 || y > WINDOW_HEIGHT || y < 0 || x + texW > WINDOW_WIDTH || y + texH > WINDOW_HEIGHT) {
        printf("ERROR: Text is out of bounds\nx: %d, y:%d\n", x, y);
        return;
    }

    SDL_Rect dstrect = {x, y, texW, texH };
    SDL_RenderCopy(win->renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

const char* ftostr(float f) {
    char* buf = alloca(sizeof(char) * 0xff);
    sprintf(buf, "%.4f", f);
    return buf;
}

const char* itostr(int d) {
    char* buf = alloca(sizeof(char) * 0xff);
    sprintf(buf, "%d", d);
    return buf;
}

const char* hextostr(int d) {
    char* buf = alloca(sizeof(char) * 0xff);
    sprintf(buf, "0x%02x", d);
    return buf;
}

const char* vI(char d) {
    char* buf = alloca(sizeof(char) * 0xff);
    sprintf(buf, "v%x:", d);
    return buf;
}

void DebugWindow(Window* win) {
    int padx = 40, pady = 60;
    SDL_SetRenderDrawColor(win->renderer, DEBUG_CLEAR_COLOR);
    SDL_RenderFillRect(win->renderer, &debugRect);
    DebugText(win, "PC:", padx, pady);
    DebugText(win, hextostr(state.pc), padx + 40, pady);
    DebugTextR(win, "I:", -padx - 70, pady);
    DebugTextR(win, hextostr(state.pc), -padx, pady);
    pady += 40;
    for (int i = 0; i < 16; i+=2) {
        DebugText(win, vI(i), padx, pady);
        DebugText(win, hextostr(state.v[i]), padx + 40, pady);
        DebugTextR(win, vI(i + 1), -padx - 70, pady);
        DebugTextR(win, hextostr(state.v[i + 1]), -padx, pady);
        pady += 24;
    }
    DebugTextB(win, "FPS:", padx, -40);
    DebugTextBR(win, itostr(FPS), -padx, -40);
    DebugTextB(win, "Delta Time:", padx, -20);
    DebugTextBR(win, ftostr(DELTA_TIME), -padx, -20);
}
#endif
