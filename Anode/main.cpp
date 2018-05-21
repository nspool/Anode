//
//  main.cpp
//  Anode
//
//  Created by nsp on 16/9/17.
//  Copyright © 2017 nspool. All rights reserved.
//

#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>

const unsigned SCREEN_WIDTH = 320;
const unsigned SCREEN_HEIGHT = 480;

const int WALL_WIDTH = 32;
const int WALL_HEIGHT = 1;
const int WALL_OFFSET = 128;

const int BRICK_WIDTH = 32;
const int BRICK_HEIGHT = 16;

const int PUCK_SIZE = 8;

const int PADDLE_WIDTH = 64;
const int PADDLE_HEIGHT = 8;
const int PADDLE_OFFSET = 50;

int main(int argc, char * argv[]) {

	/* -------------- */
    /* INITIALISATION */
    /* -------------- */
    
	// State of the board
    bool board[WALL_HEIGHT][WALL_WIDTH];
    for(int i=0; i<WALL_HEIGHT; i++) {
        for(int j=0; j<WALL_WIDTH; j++) {
            board[i][j] = false;
        }
    }
        
    // TODO: simplify SDL boilerplate
    
    SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0 || IMG_Init( IMG_INIT_PNG | IMG_INIT_JPG) < 0) {
        SDL_LogError(0, "Failed to Initialize SDL!");
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Anode", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if(window == 0 || renderer == 0) {
        SDL_LogError(0, "%s", SDL_GetError());
        return 1;
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

	int puckSign = -1;
    int paddleVelocity = 10;
	int puckVelocity = 2;

	double puckAngle = M_PI / 2;

    // Main event loop

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
                puckAngle = M_PI / 2;
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
                }
            }
        }

        if(puckInMotion) {
            
			SDL_Log("Puck Sign %d", puckSign);
            SDL_Point oldPos = {puckPos.x, puckPos.y};
            
            // 0 hard left π/2 UP, π hard right
            puckPos.y += puckSign * (int)ceil(puckVelocity * sin(puckAngle));
            puckPos.x += puckSign * (int)ceil(puckVelocity * cos(puckAngle));

            bool hitLeft = puckPos.x <= 0;
            bool hitTop = puckPos.y <= 0;
            bool hitRight =  SCREEN_WIDTH < puckPos.x + puckPos.w;
            bool hitBottom =  SCREEN_HEIGHT < puckPos.y + puckPos.h;
            bool hitPaddle = (puckPos.y > paddlePos.y && SDL_HasIntersection(&puckPos, &paddlePos));

            // Test of collided with screen
            if(hitTop || hitLeft || hitRight || hitBottom || hitPaddle) {
                
                // FIXME: clamp, don't revert
                puckPos.x = oldPos.x;
                puckPos.y = oldPos.y;

				double jitter = 0; // (rand() / (double)RAND_MAX) / 4.0;

                if(hitTop) {
                    puckSign *= -1;
                    puckAngle =  M_PI - puckAngle;
                    SDL_Log("Hit Top Wall");
                }
                
                if(hitLeft || hitRight) {
                    puckAngle = M_PI - puckAngle;
                    SDL_Log("Hit Side Wall");
                }
                
                if(hitBottom || hitPaddle) {
                    // TODO: Enter fail state
                    puckSign *= -1;
                    puckAngle = M_PI - puckAngle;
					SDL_Log("Hit Bottom");
                }
                
                if(hitPaddle) {
                    // Where on the paddle did the puck hit?
                    puckAngle = M_PI * ( puckPos.x + (puckPos.w / 2) - paddlePos.x) / paddlePos.w;
                    SDL_Log("Hit Paddle");
                }
                
                if(hitBottom) {
                    
                     paddlePos = {SCREEN_WIDTH / 2 - 32,SCREEN_HEIGHT - 50,64,8};
                     puckPos = {SCREEN_WIDTH / 2 - 16,SCREEN_HEIGHT - 58,8,8};
                    
                     puckInMotion = false;
                    
                     puckSign = -1;
                     puckAngle = M_PI / 2;
                     paddleVelocity = 20;
                     paddleInMotion = false;
                }

                // Apply jitter to exaggerate the angle
                puckAngle += (puckSign  * jitter);

            } else {
                
                /* if within area of puck
                    which board coords for current puck position?
                    if board[i][j] == false collision else next
                 */
                
                if(puckPos.y < (WALL_OFFSET + (16 * WALL_HEIGHT)) && puckPos.y > WALL_OFFSET) // 8 brick rows
                {
                    int i = (puckPos.y - WALL_OFFSET) / 16;
                    int j = (puckPos.x) / 32;
                    // SDL_Log("puck at %d %d", i, j);
                    if(board[i][j] == false) {
                        SDL_Log("Collision!");

						// FIXME: On what side ??

                        board[i][j] = true;
                        puckSign *= -1; // Collision
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
        for(int i=0; i<WALL_HEIGHT; i++) {
            for(int j=0; j<WALL_WIDTH; j++) {
                brickPos.y = 16*i + WALL_OFFSET;
                brickPos.x = 32*j;
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
