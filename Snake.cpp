
#include "Snake.h"

#include <unordered_set>
#include <iostream>

namespace snake
{
    Snake::Snake(SDL_Renderer *renderer,
                 unsigned int random_seed,
                 unsigned int width,
                 unsigned int height,
                 unsigned int start_x,
                 unsigned int start_y,
                 unsigned int min_length,
                 unsigned int apples)
                 : m_state(PLAYING)
                 , m_cells(NULL)
                 , m_snake()
                 , m_min_length(min_length)
                 , m_max_apples(apples)
                 , m_apples(apples)
                 , m_width(width)
                 , m_height(height)
                 , m_start_x(start_x)
                 , m_start_y(start_y)
                 , m_grow(0)
                 , m_rand()
    {
        if (m_width < 3)
            m_width = 3;
        if (m_height < 3)
            m_height = 3;
        if (m_start_x < 1 || m_start_x > m_width - 2)
            m_start_x = 1;
        if (m_start_y < 1 || m_start_y > m_height - 2)
            m_start_y = 1;
        if (m_apples > m_width * m_height)
            m_apples = m_width * m_height;

        m_cells = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
        if (m_cells == NULL)
        {
            std::cerr << "Failed to create game texture. ";
            std::cerr << SDL_GetError() << std::endl;
            m_state = ERROR;
            return;
        }

        m_rand.seed(random_seed);
        reset();
    }

    Snake::~Snake()
    {
        SDL_DestroyTexture(m_cells);
    }

    State Snake::state() const
    {
        return m_state;
    }

    unsigned int Snake::apples() const
    {
        return m_apples;
    }

    unsigned int Snake::length() const
    {
        return m_snake.size();
    }

    unsigned int Snake::width() const
    {
        return m_width;
    }

    unsigned int Snake::height() const
    {
        return m_height;
    }

    SDL_Texture *Snake::cells()
    {
        return m_cells;
    }

    void Snake::move(Direction direction)
    {
        if (m_state != PLAYING)
            return;
        if (m_snake.size() < 1)
        {
            std::cerr << "Snake is of size zero. How? " << std::endl;
            m_state = ERROR;
            return;
        }

        unsigned int next;
        switch (direction)
        {
        case NORTH:
            next = m_snake.back() - m_width;
            if (next > m_snake.back())
            {
                m_state = ERROR;
                return;
            }
            break;
        case SOUTH:
            next = m_snake.back() + m_width;
            if (next > m_width * m_height || next < m_snake.back())
            {
                m_state = ERROR;
                return;
            }
            break;
        case EAST:
            next = m_snake.back() + 1;
            if (next % m_width == 0 || next < m_snake.back())
            {
                m_state = ERROR;
                return;
            }
            break;
        case WEST:
            next = m_snake.back() - 1;
            if (next % m_width + 1 == 0 || next > m_snake.back())
            {
                m_state = ERROR;
                return;
            }
            break;
        case NONE:
            return;
        }

        unsigned int *pixels;
        int pitch;
        if (SDL_LockTexture(m_cells, NULL, reinterpret_cast<void **>(&pixels), &pitch) < 0)
        {
            std::cerr << "Failed to edit game board. ";
            std::cerr << "Error: " << SDL_GetError() << std::endl;
            m_state = ERROR;
            return;
        }

        switch (pixels[next])
        {
        case APPLE:
            m_apples -= 1;
            if (m_apples == 0)
                pixels[m_width / 2] = GOAL;
            m_grow += 2;
            break;
        case GOAL:
            m_state = WON;
            break;
        case WALL:
        case SNAKE:
            m_state = LOST;
            break;
        default:
            break;
        }

        if (m_grow == 0 && m_snake.size() >= m_min_length)
        {
            pixels[m_snake.front()] = EMPTY;
            m_snake.pop();
        }
        else if (m_grow > 0)
        {
            m_grow -= 1;
        }

        pixels[next] = SNAKE;
        m_snake.push(next);

        SDL_UnlockTexture(m_cells);
    }

    void Snake::reset()
    {
        if (m_state == ERROR)
            return;

        unsigned int *pixels;
        int pitch;
        if (SDL_LockTexture(m_cells, NULL, reinterpret_cast<void **>(&pixels), &pitch) < 0)
        {
            std::cerr << "Failed to edit game board. ";
            std::cerr << "Error: " << SDL_GetError() << std::endl;
            m_state = ERROR;
            return;
        }

        m_state = PLAYING;
        for (unsigned int i = 0; i < m_width * m_height; ++i)
            pixels[i] = EMPTY;
        std::queue<unsigned int> empty_queue;
        m_snake.swap(empty_queue); // Swap with empty queue
        m_apples = m_max_apples;
        m_grow = 0;

        // Generate random positions for apples
        std::unordered_set<unsigned int> apple_positions;
        for (unsigned int i = 0; i < m_apples; ++i)
        {
            unsigned int r;
            do
            {
                unsigned int rx = m_rand() % (m_width - 2) + 1;
                unsigned int ry = m_rand() % (m_height - 2) + 1;
                r = rx + m_width * ry;
            }
            while (apple_positions.count(r) != 0 || r == (m_start_x + m_start_y * m_width));
            apple_positions.insert(r);
        }

        // Set initial snake
        pixels[m_start_x + m_start_y * m_width] = SNAKE;
        m_snake.push(m_start_x + m_start_y * m_width);

        // Set walls
        for (unsigned int i = 0; i < m_width; ++i)
        {
            pixels[i] = WALL;
            pixels[i + m_width * (m_height - 1)] = WALL;
        }

        for (unsigned int i = 1; i < m_height - 1; ++i)
        {
            pixels[i * m_width] = WALL;
            pixels[(i + 1) * m_width - 1] = WALL;
        }

        // Set apples
        for (unsigned int i : apple_positions)
            pixels[i] = APPLE;

        SDL_UnlockTexture(m_cells);
    }
}
