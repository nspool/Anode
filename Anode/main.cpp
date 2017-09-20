//
//  main.cpp
//  Anode
//
//  Created by nsp on 16/9/17.
//  Copyright © 2017 nspool. All rights reserved.
//

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL_timer.h>

constexpr unsigned int SCREEN_WIDTH = 640;
constexpr unsigned int SCREEN_HEIGHT = 480;

int main(int argc, const char * argv[]) {

    // Initialization
    
    // TODO: simplify SDL boilerplate
    
    SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0 || IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG) < 0) {
        std::cerr << "Failed to Initialize SDL!" << std::endl;
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Anode", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if(window == 0 || renderer == 0) {
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_SetWindowTitle(window, "Anode");
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    
    SDL_Event e;
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    
    // Game sprites
    
    SDL_Surface* paddle = IMG_Load("paddle.png");
    SDL_Texture* paddleTex = SDL_CreateTextureFromSurface(renderer, paddle);
    SDL_Rect paddleBounds = {0,0,64,8};
    SDL_Rect puckBounds = {0,0,64,8};

    // Game State

    bool inProgress = true;
    
    SDL_Rect paddlePos = {SCREEN_WIDTH / 2 - 32,SCREEN_HEIGHT - 50,64,8};
    SDL_Rect puckPos = {SCREEN_WIDTH / 2 - 16,SCREEN_HEIGHT - 58,8,8};
    
    bool puckInMotion = false;
    float puckAngle = 0;
    float puckVelocity = 1;

    // Main event loop

    while(inProgress) {

        if(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT) {
                inProgress = false;
                break;
            }
            
            if(e.type ==  SDL_WINDOWEVENT){
                switch(e.window.event) {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        camera.w = e.window.data1;
                        camera.h = e.window.data2;
                        SDL_RenderPresent(renderer);
                        break;
                    default:
                        break;
                }
            }
        }
        
        if(keystates[SDL_SCANCODE_LEFT]) {
            paddlePos.x -= 3;
            if(paddlePos.x < 0) { paddlePos.x = 0; }
        }
        
        if(keystates[SDL_SCANCODE_RIGHT]) {
            paddlePos.x += 3;
            if((paddlePos.x + paddlePos.w) > SCREEN_WIDTH) { paddlePos.x = SCREEN_WIDTH - paddlePos.w; }
        }
        
        if(keystates[SDL_SCANCODE_SPACE]) {
            puckInMotion = true;
        }
        
        if(puckInMotion) {
            // TODO: calculate new puck position
            puckPos.y--;
        } else {
            // Keep it attached to the paddle
            puckPos.x = paddlePos.x + 28;
        }
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, paddleTex, &paddleBounds, &paddlePos);
        SDL_RenderCopy(renderer, paddleTex, &puckBounds, &puckPos); // TODO: custom texture for puck
        SDL_RenderPresent(renderer);

    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
