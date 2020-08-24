#include "term.hh"

#include <iostream>
#include <string>
#include <fstream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <csignal>
#include <vector>
#include <sstream>
#include <termios.h>

#define escape(fmt, ...)                                                \
    do {                                                                \
        printf("\e[" fmt __VA_OPT__(,) __VA_ARGS__);                    \
        fflush(stdout);                                                 \
    } while(0)

Key read_key(int fd) {
    int count;
    ioctl(fd, FIONREAD, &count);
    if (count == 0) return Key::KEY_NULL;

    int nread;
    char c, seq[3];
    while ((nread = read(fd,&c,1)) == 0);
    if (nread == -1) exit(1);

    while(1) {
        switch(c) {
        case ESC:    /* escape sequence */
            /* If this is just an ESC, we'll timeout here. */
            if (read(fd,seq,1) == 0) return Key::ESC;
            if (read(fd,seq+1,1) == 0) return Key::ESC;

            /* ESC [ sequences. */
            if (seq[0] == '[') {
                if (seq[1] >= '0' && seq[1] <= '9') {
                    /* Extended escape, read additional byte. */
                    if (read(fd,seq+2,1) == 0) return Key::ESC;
                    if (seq[2] == '~') {
                        switch(seq[1]) {
                        case '3': return Key::DEL_KEY;
                        case '5': return Key::PAGE_UP;
                        case '6': return Key::PAGE_DOWN;
                        }
                    }
                } else {
                    switch(seq[1]) {
                    case 'A': return Key::ARROW_UP;
                    case 'B': return Key::ARROW_DOWN;
                    case 'C': return Key::ARROW_RIGHT;
                    case 'D': return Key::ARROW_LEFT;
                    case 'H': return Key::HOME_KEY;
                    case 'F': return Key::END_KEY;
                    }
                }
            }

            /* ESC O sequences. */
            else if (seq[0] == 'O') {
                switch(seq[1]) {
                case 'H': return Key::HOME_KEY;
                case 'F': return Key::END_KEY;
                }
            }
            break;
        default:
            return (Key)c;
        }
    }
}

struct std::pair<int, int> get_term_size() {
    struct winsize window_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size);
    return { window_size.ws_row, window_size.ws_col };
}

/****************************************************************
 * Terminal interaction:
 ****************************************************************/
void set_cursor_position(int row, int col) {
    escape("\e[%d;%df", row + 1, col + 1);
}

void show_cursor(bool show) {
    if (show) escape("?25h");
    else escape("?25l");
}

void clear(ClearOpt what) {
    switch (what) {
    case ClearOpt::LineRight: escape("0K"); break;
    case ClearOpt::LineLeft: escape("1K"); break;
    case ClearOpt::Line: escape("2K"); break;
    case ClearOpt::ScreenDown: escape("0J"); break;
    case ClearOpt::ScreenUp: escape("1J"); break;
    case ClearOpt::Screen: escape("2J"); break;
    }
}

void clear_line() {
    escape("2J");
}


/****************************************************************
 * Raw mode.
 ****************************************************************/
bool raw_mode(bool enable) {
    static termios prev_termios;
    if (enable) {
        if (!isatty(STDIN_FILENO)) return false;
        atexit(disable_raw_mode);
        if (tcgetattr(STDIN_FILENO, &prev_termios) == -1) return false;
        struct termios raw = prev_termios;  /* modify the original mode */
        /* input modes: no break, no CR to NL, no parity check, no strip char,
         * no start/stop output control. */
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        /* output modes - disable post processing */
        raw.c_oflag &= ~(OPOST);
        /* control modes - set 8 bit chars */
        raw.c_cflag |= (CS8);
        /* local modes - choing off, canonical off, no extended functions,
         * no signal chars (^Z,^C) */
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        /* control chars - set return condition: min number of bytes and timer. */
        raw.c_cc[VMIN] = 0; /* Return each byte, or zero for timeout. */
        raw.c_cc[VTIME] = 1; /* 100 ms timeout (unit is tens of second). */

        /* put terminal in raw mode after flushing */
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) return false;
        return true;
    } else {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &prev_termios);
        return true;
    }
}

bool enable_raw_mode() {
    return raw_mode(true);
}

void disable_raw_mode() {
    raw_mode(false);
}
