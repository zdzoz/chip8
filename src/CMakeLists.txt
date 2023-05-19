add_executable(chip8 main.c window.c window.h emulator.c emulator.h stack.h stack.c)

target_link_libraries(chip8 SDL2::SDL2 SDL2_ttf::SDL2_ttf)
