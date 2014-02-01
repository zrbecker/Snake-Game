
#ifndef SNAKE_H
#define SNAKE_H

#include <queue>
#include <vector>
#include <random>

#include <iostream>

namespace snake
{
    enum Cell
    {
        EMPTY = 0x000000FF, // Black
        SNAKE = 0xFFFF00FF, // Yellow
        APPLE = 0xFF0000FF, // Red
        WALL = 0x0000FFFF,  // Blue
        GOAL = 0x00FF00FF   // Green
    };

    enum Direction
    {
        NONE,
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    enum State
    {
        WON,
        LOST,
        PLAYING,
        ERROR
    };

    class Snake
    {
    public:
        /*
        * Initializes a new game of snake.
        *
        * random_seed - used to generate apples
        * width - width of board
        * height - height of board
        * x - starting x position of snake
        * y - starting y position of snake
        * min_length - minimum length of snake
        * apples - number of apples on board
        */
        Snake(unsigned int random_seed = 0,
              unsigned int width = 20,
              unsigned int height = 20,
              unsigned int x = 9,
              unsigned int y = 9,
              unsigned int min_length = 5,
              unsigned int apples = 10);

        /*
        * Returns the current state of the game.
        *
        * WON, LOST, PLAYING, ERROR
        */
        State state() const;

        /*
        * Returns the number of apples remaining on the board.
        */
        unsigned int apples() const;

        /*
        * Returns the current length of the snake.
        */
        unsigned int length() const;

        /*
        * Returns the width of the board.
        */
        unsigned int width() const;

        /*
        * Returns the height of the board.
        */
        unsigned int height() const;

        /*
        * Returns std::vector<Cell> representing the current board
        */
        const std::vector<Cell> &cells() const;

        /*
        * Returns std::queue<unsigned int> representing the indices
        * of the board which contain the snake.
        */
        const std::queue<unsigned int> &snake() const;

        /*
        * Moves the snake on the board. This function does nothing
        * if state is not PLAYING.
        */
        void move(Direction direction);

        /*
        * Resets the state of the game.
        */
        void reset();

    private:
        State m_state;
        std::vector<Cell> m_cells;
        std::queue<unsigned int> m_snake;
        unsigned int m_min_length;
        unsigned int m_max_apples;
        unsigned int m_apples;
        unsigned int m_width;
        unsigned int m_height;
        unsigned int m_start_x;
        unsigned int m_start_y;
        unsigned int m_grow;
        std::mt19937 m_rand;
    };
}

#endif
