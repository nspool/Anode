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

    std::cout << "Welcome to the game : n )\n";
    
    // Initialization
    
    // Keeping the state of the board in an array.. for now
    
    bool board[32];
    
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
    
    SDL_Rect brickPos = {0,100,32,16};

    // Game State

    bool inProgress = true;
    
    SDL_Rect paddlePos = {SCREEN_WIDTH / 2 - 32,SCREEN_HEIGHT - 50,64,8};
    SDL_Rect puckPos = {SCREEN_WIDTH / 2 - 16,SCREEN_HEIGHT - 58,8,8};
    
    bool puckInMotion = false;
    
    float puckSign = -1;
    float puckAngle = M_PI_2;
    float puckVelocity = 10;
    float paddleVelocity = 10;
    bool paddleInMotion = false;

    // Main event loop

    std::cout << "Starting event loop ... \n";
    
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
                
        bool scanLeft = keystates[SDL_SCANCODE_LEFT];
        bool scanRight = keystates[SDL_SCANCODE_RIGHT];
        bool scanSpace = keystates[SDL_SCANCODE_SPACE];
        
        if(paddleInMotion && !scanLeft && !scanRight) {
            paddleInMotion = false;
            // Set puck back to default values
            if(!puckInMotion) {
                puckAngle = M_PI_2;
                puckVelocity = 7;
            }
        }
        
        if(!puckInMotion && scanSpace) {
            puckInMotion = true;
        }
        
        if(scanLeft) {
            paddlePos.x -= paddleVelocity;
            if(paddlePos.x < 0) {
                paddlePos.x = 0;
            } else {
                if(!puckInMotion){
                    paddleInMotion = true;
                    puckAngle = 0.1;
                    puckVelocity = 10;
                }
            }
        }
        
        if(scanRight) {
            paddlePos.x += paddleVelocity;
            if((paddlePos.x + paddlePos.w) > SCREEN_WIDTH) {
                paddlePos.x = SCREEN_WIDTH - paddlePos.w;
            } else {
                if(!puckInMotion) {
                    paddleInMotion = true;
                    puckAngle = M_PI - 0.1;
                    puckVelocity = 10;
                }
            }
        }

        if(puckInMotion) {
            
            SDL_Point oldPos = {puckPos.x, puckPos.y};
            
            // 0 hard left π/2 UP, π hard right
            puckPos.y += puckSign * ceil(puckVelocity * sin(puckAngle));
            puckPos.x += puckSign * ceil(puckVelocity * cos(puckAngle));

            bool hitLeft = puckPos.x <= 0;
            bool hitTop = puckPos.y <= 0;
            bool hitRight =  SCREEN_WIDTH < puckPos.x + puckPos.w;
            bool hitBottom =  SCREEN_HEIGHT < puckPos.y + puckPos.h;
            bool hitPaddle = SDL_HasIntersection(&puckPos, &paddlePos);
            

            // Test of collided with screen
            if(hitTop || hitLeft || hitRight || hitBottom || hitPaddle) {
                
                // FIXME: clamp, don't revert
                puckPos.x = oldPos.x;
                puckPos.y = oldPos.y;
                
                if(hitTop) {
                    puckSign *= -1;
                    puckAngle =  M_PI - puckAngle;
                    std::cout << "Hit Top Wall\n";
                }
                
                if(hitLeft || hitRight) {
                    puckAngle = M_PI - puckAngle;
                    std::cout << "Hit Side Wall\n";
                }
                
                if(hitBottom || hitPaddle) {
                    // TODO
                    puckSign *= -1;
                    puckAngle = M_PI - puckAngle;
                }
                
                if(hitPaddle) {
                    std::cout << "Hit Paddle\n";
                    // Where on the paddle did the puck hit?
                    puckAngle = M_PI * ( puckPos.x + (puckPos.w / 2) - paddlePos.x) / paddlePos.w;;
                }
            } else {
                // FIX THIS:
                for(int i=0; i<32; i++) {
                    brickPos.x = 32*i;
                    if(!board[i] && SDL_HasIntersection(&puckPos, &brickPos)) {
                        std::cout << "Hit Brick\n";

                        // TODO: rebound after bounce
                        board[i] = true;
                    }
                }
            }
            
        } else {
            // Keep it attached to the paddle
            puckPos.x = paddlePos.x + 28;
        }
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, paddleTex, &paddleBounds, &paddlePos);
        SDL_RenderCopy(renderer, paddleTex, &puckBounds, &puckPos); // TODO: custom texture for puck

        for(int i=0; i<32; i++) {
            brickPos.x = 32*i;
            if(!board[i]) {
                SDL_RenderCopy(renderer, paddleTex, &puckBounds, &brickPos); // TODO: custom texture for bricks
            }
        }

        SDL_RenderPresent(renderer);

    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
