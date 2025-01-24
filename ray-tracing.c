/* ray-tracing.c */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600
#define fps 30
#define delay 1000 / fps

double* calculated_slopes;
uint16_t slope_count = 0;

struct Circle {
    double x;
    double y;
    double radius;
};

void drawCircle(SDL_Renderer* renderer, struct Circle circle){
    double radius_squared = circle.radius * circle.radius;
    for(double x = circle.x - circle.radius; x <= circle.x + circle.radius; x++){
        for(double y = circle.y - circle.radius; y <= circle.y + circle.radius; y++){
            double dist_squared = ((x - circle.x) * (x - circle.x)) + ((y - circle.y) * (y - circle.y));
            if(dist_squared <= radius_squared){
                SDL_Rect rect = {x, y, 1, 1};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

uint8_t checkCircleColliding(struct Circle circle1, struct Circle circle2) {
    double radius_sum_squared = (circle1.radius + circle2.radius) * (circle1.radius + circle2.radius);
    double dist_squared = ((circle1.x - circle2.x) * (circle1.x - circle2.x)) + ((circle1.y - circle2.y) * (circle1.y - circle2.y));
    if(dist_squared <= radius_sum_squared)
        return 1;
    return 0;
}

uint8_t checkColliding(struct Circle circle1, double x, double y) {
    double radius_squared = (circle1.radius * circle1.radius);
    double dist_squared = ((circle1.x - x) * (circle1.x - x)) + ((circle1.y - y) * (circle1.y - y));
    if(dist_squared <= radius_squared)
        return 1;
    return 0;
}

double f(double x, double m, int circlex, int circley) {
    return m * (circlex-x) + circley;
}

void calculateSlopes(int value) {
    slope_count = 360 / value;
    calculated_slopes = (double*) malloc(slope_count * sizeof(double));
    printf("Degree: %d\nRay count: %u\n\n", value, (unsigned int)slope_count);
    for(uint16_t i = 0; i < slope_count; i++){
        calculated_slopes[i] = tan(i * value * M_PI / 180);
    }
}

int main(int argc, char* argv[]) {

    int value = 15;
    if (argc == 2) {
        if (atoi(argv[1]) <= 0) {
            printf("Invalid angle.");
            return -1;
        } else {
            value = atoi(argv[1]);
        }
    }

    calculateSlopes(value);

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

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Rect blank_screen = {0, 0, WIDTH, HEIGHT};
    
    struct Circle circle = {400, 300, 50};
    struct Circle static_circle = {600, 200, 150};

    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                free(calculated_slopes);
            } else if (event.type == SDL_MOUSEMOTION && event.motion.state) {
                circle.x = event.motion.x;
                circle.y = event.motion.y;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for(uint16_t i = 0; i < slope_count; i++) {
            double angle = calculated_slopes[i];
            for(double x = circle.x; x < WIDTH; x+=.1) {
                double y = f(x, angle, circle.x, circle.y);
                SDL_Rect rect = {x, y, 1, 1};
                if(checkColliding(static_circle, x, y)) break;
                SDL_RenderFillRect(renderer, &rect);
            }
            for(double x = circle.x; x > 0; x-=.1) {
                double y = f(x, angle, circle.x, circle.y);
                SDL_Rect rect = (SDL_Rect) {x, y, 1, 1};
                if(checkColliding(static_circle, x, y) || (y > HEIGHT)) break;
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        if(checkCircleColliding(circle, static_circle)) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            drawCircle(renderer, circle);
            drawCircle(renderer, static_circle);
        } else {
            drawCircle(renderer, circle);
            drawCircle(renderer, static_circle);
        }

        SDL_RenderPresent(renderer);
        //SDL_Delay(delay);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
