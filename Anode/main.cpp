//
//  main.cpp
//  Anode
//
//  Created by nsp on 16/9/17.
//  Copyright © 2017 nspool. All rights reserved.
//

// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-Wdocumentation"

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL_timer.h>

constexpr unsigned int SCREEN_WIDTH = 640;
constexpr unsigned int SCREEN_HEIGHT = 480;

int main(int argc, const char * argv[]) {

    // Initialization
    
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
    
    // Main event loop
    
    bool quit = false;
    
    do {
        
        if(SDL_PollEvent(&e) != 0)
        {
            switch(e.type){
                case SDL_QUIT:
                    quit = true;
                    break;
                    //Window event occured
                case SDL_WINDOWEVENT:
                    switch( e.window.event )
                {
                        //Get new dimensions and repaint on window size change
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        camera.w = e.window.data1;
                        camera.h = e.window.data2;
                        SDL_RenderPresent(renderer);
                        break;
                    default:
                        break;
                }
                    break;
            }
        }
        
        if(keystates[SDL_SCANCODE_LEFT]) {
            printf("left!\n");
        }
        
        if(keystates[SDL_SCANCODE_RIGHT]) {
            printf("right!\n");
        }
        
        SDL_RenderClear(renderer);

        
        SDL_RenderPresent(renderer);

    } while(!quit);
    
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
