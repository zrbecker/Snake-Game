
#include "Snake.h"

using namespace snake;

#include <ctime>
#include <iostream>
#include <sstream>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

const unsigned int SCREEN_WIDTH = 500;
const unsigned int SCREEN_HEIGHT = 500;

const unsigned int GAME_WIDTH = 30;
const unsigned int GAME_HEIGHT = 30;

const unsigned int GAME_START_X = 14;
const unsigned int GAME_START_Y = 14;

const unsigned int GAME_SNAKE_MIN_LENGTH = 5;
const unsigned int GAME_APPLES = 50;

const unsigned int GAME_INITIAL_SPEED = 300;
const double GAME_INCREASE_SPEED_FACTOR = 0.7;

bool Initialize(SDL_Window **window, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not be initialized. ";
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *window = SDL_CreateWindow("Snake",
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL)
    {
        SDL_Quit();
        std::cerr << "SDL window could not be created. ";
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL)
    {
        SDL_DestroyWindow(*window);
        *window = NULL;
        SDL_Quit();
        std::cerr << "SDL renderer could not be created. ";
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Clean(SDL_Window **window, SDL_Renderer **renderer)
{
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(*window);
    *renderer = NULL;
    *window = NULL;

    SDL_Quit();
}

int main(int argc, char *argv[])
{

    SDL_Window *window;
    SDL_Renderer *renderer;

    if (!Initialize(&window, &renderer))
    {
        std::cerr << "Failed to initialize." << std::endl;
        return 1;
    }

    Snake game(renderer, SDL_GetTicks() ^ std::time(0), GAME_WIDTH, GAME_HEIGHT, GAME_START_X, GAME_START_Y, GAME_SNAKE_MIN_LENGTH, GAME_APPLES);
    Direction direction = NORTH;
    Direction lastMove = NONE;
    bool quit = false;
    SDL_Event e;
    unsigned int level = 1;
    unsigned int lastUpdate = SDL_GetTicks();
    unsigned int speed = GAME_INITIAL_SPEED;

    while (!quit)
    {
        if (game.state() == ERROR)
        {
            SDL_ShowSimpleMessageBox(0, "Error", "There was an error. Closing game.", NULL);
            break;
        }

        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    if (lastMove != SOUTH)
                        direction = NORTH;
                    break;
                case SDLK_DOWN:
                    if (lastMove != NORTH)
                        direction = SOUTH;
                    break;
                case SDLK_LEFT:
                    if (lastMove != EAST)
                        direction = WEST;
                    break;
                case SDLK_RIGHT:
                    if (lastMove != WEST)
                        direction = EAST;
                    break;
                case SDLK_r:
                    level = 1;
                    speed = GAME_INITIAL_SPEED;
                    direction = NORTH;
                    game.reset();
                    break;
                }
                break;
            }
        }

        if (game.state() == WON)
        {
            level += 1;
            speed = static_cast<unsigned int>(speed * GAME_INCREASE_SPEED_FACTOR);
            direction = NORTH;
            game.reset();
        }
        else if (game.state() == LOST)
        {
            std::stringstream message;
            message << "You got to level " << level << "!";
            SDL_ShowSimpleMessageBox(0, "Game Over", message.str().c_str(), NULL);
            level = 1;
            speed = GAME_INITIAL_SPEED;
            direction = NORTH;
            game.reset();
        }

        unsigned int now = SDL_GetTicks();
        if (now - lastUpdate > speed)
        {
            game.move(direction);
            lastMove = direction;
            lastUpdate = now;

            SDL_RenderCopy(renderer, game.cells(), NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    Clean(&window, &renderer);

    return 0;
}
