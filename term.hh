#pragma once

#include <utility>

enum Key : int {
        KEY_NULL = 0,       /* NULL */
        CTRL_A = 1,         /* Ctrl-a */
        CTRL_C = 3,         /* Ctrl-c */
        CTRL_D = 4,         /* Ctrl-d */
        CTRL_E = 5,         /* Ctrl-e */
        CTRL_F = 6,         /* Ctrl-f */
        CTRL_H = 8,         /* Ctrl-h */
        CTRL_K = 11,        /* Ctrl-k */
        TAB = 9,            /* Tab */
        CTRL_L = 12,        /* Ctrl+l */
        ENTER = 13,         /* Enter */
        CTRL_Q = 17,        /* Ctrl-q */
        CTRL_S = 19,        /* Ctrl-s */
        CTRL_U = 21,        /* Ctrl-u */
        ESC = 27,           /* Escape */
        BACKSPACE =  127,   /* Backspace */
        /* The following are just soft codes, not really reported by the
         * terminal directly. */
        ARROW_LEFT = 1000,
        ARROW_RIGHT,
        ARROW_UP,
        ARROW_DOWN,
        DEL_KEY,
        HOME_KEY,
        END_KEY,
        PAGE_UP,
        PAGE_DOWN
};

Key read_key(int fd);

struct std::pair<int, int> get_term_size();

/****************************************************************
 * Terminal interaction:
 ****************************************************************/
// void cursor_left(int n=1);
// void cursor_right(int n=1);
// void cursor_up(int n=1);
// void cursor_down(int n=1);

void set_cursor_position(int row, int col);

void show_cursor(bool show);

enum class ClearOpt {
    // Clear line right of cursor.
    LineRight,
    // Clear line left of cursor.
    LineLeft,
    // Clear entire line.
    Line,
    // Clear screen below cursor.
    ScreenDown,
    // Clear screen above cursor.
    ScreenUp,
    // Clear entire screen.
    Screen
};

void clear(ClearOpt what);


/****************************************************************
 * Raw mode.
 ****************************************************************/
bool raw_mode(bool enable);

bool enable_raw_mode();

void disable_raw_mode();
