#include "window.h"

#include "emulator.h"

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
    };


    win->font = TTF_OpenFont("fonts/Cozette/CozetteVector.ttf", 18);
    if (win->font == NULL) {
        printf("Failed to read font: %s\n", TTF_GetError());
        exit(-1);
    }

    return TRUE;
}

void CleanWindow(Window* win) {
    TTF_CloseFont(win->font);
    TTF_Quit();

    SDL_DestroyRenderer(win->renderer);
    SDL_DestroyWindow(win->window);
    SDL_Quit();
}

void ClearScreen(Window* win) {
    SDL_RenderPresent(win->renderer);
    SDL_SetRenderDrawColor(win->renderer, CLEAR_COLOR);
    SDL_RenderClear(win->renderer);
}

void RenderText_(Window* win, const char* text, int x, int y, int bot, int right, int debug) {
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



#ifndef NDEBUG
const char* ftostr(float f) {
    char* buf = alloca(sizeof(char) * 0xff);
    sprintf(buf, "%.4f", f);
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

void DebugWindow(Window* win, void* state) {
    int padx = 40, pady = 20;
    State* st = state;
    SDL_SetRenderDrawColor(win->renderer, DEBUG_CLEAR_COLOR);
    SDL_RenderFillRect(win->renderer, &debugRect);
    pady += 40;
    RenderTextDB(win, "PC:", padx, pady);
    RenderTextDB(win, hextostr(st->pc), padx + 40, pady);
    RenderTextDBRight(win, "I:", -padx - 70, pady);
    RenderTextDBRight(win, hextostr(st->pc), -padx, pady);
    pady += 40;
    for (int i = 0; i < 16; i+=2) {
        RenderTextDB(win, vI(i), padx, pady);
        RenderTextDB(win, hextostr(st->v[i]), padx + 40, pady);
        RenderTextDBRight(win, vI(i + 1), -padx - 70, pady);
        RenderTextDBRight(win, hextostr(st->v[i + 1]), -padx, pady);
        pady += 24;
    }

    RenderTextDBBotRight(win, "Delta Time:", -padx - 70, -20);
    RenderTextDBBotRight(win, ftostr(DELTA_TIME), -padx, -20);
}
#endif
