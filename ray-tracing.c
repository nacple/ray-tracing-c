/* ray-tracing.c */

#include <SDL2/SDL.h>
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600
#define COLOR_WHITE 0xFFFFFF

struct Circle {
    double x;
    double y;
    double radius;
};

void drawCircle(SDL_Surface* surface, struct Circle circle){
    double radius_squared = circle.radius * circle.radius;
    for(double x = circle.x - circle.radius; x <= circle.x + circle.radius; x++){
        for(double y = circle.y - circle.radius; y <= circle.y + circle.radius; y++){
            double dist_squared = ((x - circle.x) * (x - circle.x)) + ((y - circle.y) * (y - circle.y));
            if(dist_squared <= radius_squared){
                SDL_Rect rect = {x, y, 1, 1};
                SDL_FillRect(surface, &rect, COLOR_WHITE);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize: %s\n", SDL_GetError());
        return 1;
    }

    // Create the window
    SDL_Window* window = SDL_CreateWindow("Ray tracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    if (window == NULL) {
        printf("Window could not be created: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    
    struct Circle circle = {400, 300, 200};
    drawCircle(surface, circle);
    SDL_UpdateWindowSurface(window);

    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
