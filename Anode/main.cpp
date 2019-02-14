//
//  main.cpp
//  Anode
//
//  Created by nsp on 16/9/17.
//  Copyright © 2017 nspool. All rights reserved.
//

#include <iostream>

#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>

const unsigned SCREEN_WIDTH = 320;
const unsigned SCREEN_HEIGHT = 480;

const int WALL_BREADTH = 32;
const int WALL_DEPTH = 8;
const int WALL_OFFSET = 128;

const int BRICK_WIDTH = 32;
const int BRICK_HEIGHT = 16;

const int PADDLE_WIDTH = 64;
const int PADDLE_HEIGHT = 8;
const int PADDLE_OFFSET = 50;

const int PUCK_SIZE = 8;
const double PUCK_INITIAL_ANGLE = -3*M_PI/4; // << -M_PI  ^-2^^  0 >>

typedef enum { BOTTOM, TOP, LEFT, RIGHT, PADDLE, NONE } CollisionType;

CollisionType testCollision(SDL_Rect paddlePos, SDL_Rect puckPos)
{
    if (puckPos.x < 1) { return LEFT; }
    
    bool hitTop = puckPos.y < 1;
    if (hitTop) { return TOP; }
    
    bool hitRight = SCREEN_WIDTH < (puckPos.x + puckPos.w);
    if (hitRight) { return RIGHT; }
    
    bool hitBottom = SCREEN_HEIGHT < (puckPos.y + puckPos.h);
    if (hitBottom) { return BOTTOM; }
    
    bool hitPaddle = puckPos.y > SCREEN_HEIGHT - (PADDLE_OFFSET + PADDLE_HEIGHT)
    && puckPos.y < SCREEN_HEIGHT - PADDLE_OFFSET
    && puckPos.x >= paddlePos.x
    && puckPos.x <= (paddlePos.x + PADDLE_WIDTH);
    
    if (hitPaddle) { return PADDLE; }
    
    return NONE;
}

int main(int argc, char * argv[]) {
    
    /* -------------- */
    /* INITIALISATION */
    /* -------------- */
    
    // State of the board
    bool board[WALL_DEPTH][WALL_BREADTH];
    for(int i=0; i<WALL_DEPTH; i++) {
        for(int j=0; j<WALL_BREADTH; j++) {
            board[i][j] = false;
        }
    }
    
    // TODO: simplify SDL boilerplate
    
    SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Window* window  = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    try
    {
        if((SDL_Init(SDL_INIT_EVERYTHING) < 0) ||
           IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG) < 0) {
            throw;
        }
        
        window = SDL_CreateWindow("Anode", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        
        if(window == 0 || renderer == 0) {
            SDL_LogError(0, "%s", SDL_GetError());
            throw;
        }
    }
    catch(...)
    {
        std::cerr << "Failed to Initialize SDL!\n";
    }
    
    SDL_SetWindowTitle(window, "Anode");
    SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0xff, 0xff);
    
    SDL_Event e;
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    
    // Game sprites
    
    SDL_Surface* paddle = IMG_Load("paddle.png");
    SDL_Texture* paddleTex = SDL_CreateTextureFromSurface(renderer, paddle);
    SDL_Rect paddleBounds = {0,0,PADDLE_WIDTH,PADDLE_HEIGHT};
    
    SDL_Surface* puck = IMG_Load("puck.png");
    SDL_Texture* puckTex = SDL_CreateTextureFromSurface(renderer, puck);
    SDL_Rect puckBounds = {0,0,PUCK_SIZE,PUCK_SIZE };
    
    SDL_Surface* brick = IMG_Load("brick.png");
    SDL_Texture* brickTex = SDL_CreateTextureFromSurface(renderer, brick);
    SDL_Rect brickBounds = {0,0,BRICK_WIDTH,BRICK_HEIGHT};
    SDL_Rect brickPos = {0,0,BRICK_WIDTH,BRICK_HEIGHT };
    
    // Game State
    
    int score = 0;
    
    bool inProgress = true;
    
    SDL_Rect paddlePos = {SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - PADDLE_OFFSET, paddleBounds.w, paddleBounds.h};
    SDL_Rect puckPos = {SCREEN_WIDTH / 2 - PADDLE_WIDTH / 4, SCREEN_HEIGHT - (PADDLE_OFFSET + PADDLE_HEIGHT), puckBounds.w, puckBounds.h};
    
    bool puckInMotion = false;
    bool paddleInMotion = false;
    
    int paddleVelocity = 10;
    int puckVelocity = 2;
    
    double puckAngle = PUCK_INITIAL_ANGLE;
    
    /* ----------------*/
    /* MAIN EVENT LOOP */
    /* ----------------*/
    
    SDL_Log("Starting event loop ...");
    
    while(inProgress) {
        
        SDL_Delay(1);
        
        // TODO: Use an SDL_WaitEvent when waiting for space to start game
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
                puckAngle = PUCK_INITIAL_ANGLE;
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
                //if(!puckInMotion){
                //    paddleInMotion = true;
                //    puckAngle = 0.1;
                //}
            }
        }
        
        if(scanRight) {
            paddlePos.x += paddleVelocity;
            if((paddlePos.x + paddlePos.w) > SCREEN_WIDTH) {
                paddlePos.x = SCREEN_WIDTH - paddlePos.w;
            }
        }
        
        if(puckInMotion) {
            
            // FIXME: it shouldn't revert to the previous position,
            // but calculate the correct spot where the path of the puck
            // intersects with the object
            
            SDL_Point oldPos = {puckPos.x, puckPos.y};
            
            puckPos.y += (int)(puckVelocity * sin(puckAngle));
            puckPos.x += (int)(puckVelocity * cos(puckAngle));
            
            if (puckPos.y < 0) { puckPos.y = 0; }
            if (puckPos.x < 0) { puckPos.x = 0; }
            
            // TODO: Make this an enum type
            
            CollisionType t = testCollision(paddlePos, puckPos);
            
            // Collision Detection
            if(t != NONE) {
                
                // FIXME: clamp, don't revert
                puckPos.x = oldPos.x;
                puckPos.y = oldPos.y;
                
                if(t == TOP) {
                    SDL_Log("[TOP] @ %f", puckAngle);
                    puckAngle = -puckAngle;
                    SDL_Log("[TOP] @ %f", puckAngle);
                }
                
                if (t == LEFT) {
                    SDL_Log("[LEFT] @ %f", puckAngle);
                    puckAngle += (puckAngle < 0 ? 1 : -1) * (M_PI / 2);
                    SDL_Log("[LEFT] @ %f", puckAngle);
                }
                
                // OK ?
                if(t == RIGHT) {
                    SDL_Log("[RIGHT]");
                    puckAngle += (puckAngle < 0 ? -1 : 1) * (M_PI / 2);
                }
                
                if(t == BOTTOM) {
                    // TODO: Enter fail state
                    puckAngle += M_PI / 2;
                    SDL_Log("Hit Bottom");
                }
                
                if(t == PADDLE) {
                    // Where on the paddle did the puck hit?
                    puckAngle = -puckAngle;
                    SDL_Log("Hit Paddle");
                }
                
                if(t == BOTTOM) {
                    
                    paddlePos = {SCREEN_WIDTH / 2 - 32,SCREEN_HEIGHT - 50,64,8};
                    puckPos = {SCREEN_WIDTH / 2 - 16,SCREEN_HEIGHT - 58,8,8};
                    
                    puckInMotion = false;                    
                    
                    puckAngle += M_PI / 2;
                    // paddleVelocity = 20;
                    paddleInMotion = false;
                }
                
                // Apply jitter to exaggerate the angle
                // double jitter = 0; // (rand() / (double)RAND_MAX) / 4.0;
                // puckAngle += (puckSign  * jitter);
                
            } else {
                
                /* if within area of puck
                 which board coords for current puck position?
                 if board[i][j] == false collision else next
                 */
                
                if(puckPos.y < (WALL_OFFSET + (BRICK_HEIGHT * WALL_DEPTH)) && puckPos.y > WALL_OFFSET) // 8 brick rows
                {
                    int i = (puckPos.y - WALL_OFFSET) / BRICK_HEIGHT;
                    int j = (puckPos.x) / BRICK_WIDTH;
                    if(board[i][j] == false) {
                        
                        //int by = BRICK_HEIGHT * i + WALL_OFFSET;
                        //int bx = BRICK_WIDTH * j;
                        
                        SDL_Log("[BREAK] %d %d", i, j);
                        puckAngle = -puckAngle;
                        board[i][j] = true;
                        score++;
                    }
                }
            }
            
        } else {
            // Keep it attached to the paddle
            puckPos.x = paddlePos.x + paddlePos.w / 2 - puckBounds.w / 2;
            SDL_Delay(10);
        }
        
        SDL_RenderClear(renderer);
        
        SDL_RenderCopy(renderer, puckTex, &puckBounds, &puckPos); // TODO: custom texture for puck
        SDL_RenderCopy(renderer, paddleTex, &paddleBounds, &paddlePos);
        
        // FIX THIS:
        for(int i=0; i<WALL_DEPTH; i++) {
            for(int j=0; j<WALL_BREADTH; j++) {
                brickPos.y = BRICK_HEIGHT*i + WALL_OFFSET;
                brickPos.x = BRICK_WIDTH*j;
                if(!board[i][j]) {
                    SDL_RenderCopy(renderer, brickTex, &brickBounds, &brickPos);
                }
            }
        }
        
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
