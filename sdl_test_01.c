//SDL (Simple DirectMedia Layer)
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// Function to initialize SDL
bool init_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    return true;
}
// Function to create a window
SDL_Window* create_window(const char* title, int width, int height) {
    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        return NULL;
    }
    return window;
}
// Function to handle events
void handle_events(bool* running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }
    }
}
// Function to clean up SDL
void cleanup_sdl(SDL_Window* window) {
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}
// Main function
int main() {
    if (!init_sdl()) {
        return EXIT_FAILURE;
    }

    SDL_Window* window = create_window("SDL Test", 800, 600);
    if (!window) {
        cleanup_sdl(window);
        return EXIT_FAILURE;
    }

    bool running = true;
    while (running) {
        handle_events(&running);
        SDL_Delay(16); // Roughly 60 FPS
    }

    cleanup_sdl(window);
    return EXIT_SUCCESS;
}
// Compile with: gcc sdl_test_01.c -o sdl_test_01 -lSDL2
// Run with: ./sdl_test_01
// Note: Ensure you have the SDL2 development libraries installed on your system.
// For example, on Ubuntu, you can install them using:
// sudo apt-get install libsdl2-dev
// On Windows, you may need to download the SDL2 development libraries and set up your compiler accordingly.
// This code initializes SDL, creates a window, and handles basic events like quitting the application.
// It runs a simple event loop until the user closes the window.
// This is a basic example to get you started with SDL in C.
// You can expand this code to include rendering graphics, handling input, and more as you learn SDL.
// This code is a simple SDL application that initializes SDL, creates a window, and handles events.
// It serves as a starting point for learning SDL in C.
// Make sure to link against the SDL2 library when compiling.
// This code is a simple SDL application that initializes SDL, creates a window, and handles events.
// It serves as a starting point for learning SDL in C.