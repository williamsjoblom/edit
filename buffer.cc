#include <unistd.h>
#include <assert.h>
#include <limits>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <regex>

#include "buffer.hh"
#include "frame.hh"


void Buffer::cursor_up() { _row--; clamp_cursor(); }
void Buffer::cursor_down() { _row++; clamp_cursor(); }
void Buffer::cursor_left() { _col--; clamp_cursor(); }
void Buffer::cursor_right() { _col++; clamp_cursor(); }

const std::string keywords[] {
    "if", "do", "while", "switch", "case"
};

bool is_keyword(std::string& s) {
    return s == "if" || s == "else" || s == "do" || s == "while" || s == "switch"
        || s == "case"|| s == "default" || s == "break" || s == "continue"
        || s == "for" || s == "return";
}

bool is_comment(std::string& s) {
    return s.size() >= 2 && s[0] == '/' && s[1] == '/';
}

bool is_type(std::string& s) {
    return s == "void" || s == "bool" || s == "int";
}

bool is_cpp(std::string& s) {
    return s.size() > 0 && s[0] == '#';
}

bool is_special_literal(std::string& s) {
    return s == "true" || s == "false";
}

void print_highlighted(std::string line) {
    // std::istringstream iss(line);
    // std::copy(std::istream_iterator<std::string>(iss),
    //           std::istream_iterator<std::string>(),
    //           std::ostream_iterator<std::string>(std::cout, " "));

    const std::regex token_rx(
        "[a-zA-Z0-9_]?("
        // Keywords.
        "(if)|(else)|(do)|(while)|(switch)|(case)|(default)|(break)|(continue)|(for)|(return)|"
        // Comments.
        "(//.*$)|"
        // C preprocessor.
        "(\\#.* )|"
        // Types.
        "(void)|(unsigned)|(char)|(bool)|(short)|(int)|(long)|(float)|(double)|"
        // Special values.
        "(true)|(false)"
        ")[a-zA-Z0-9_]?"
        );
    std::vector<std::string> tokens;
    std::remove_copy_if(
        std::sregex_token_iterator(line.begin(), line.end(), token_rx, {-1, 0}),
        std::sregex_token_iterator(),
        std::back_inserter(tokens),
        [](std::string const &s) { return s.empty(); });
    for (auto& token : tokens) {
        if (is_keyword(token)) {
            printf("\e[32m%s\e[0m", token.c_str());
        } else if (is_comment(token)) {
            printf("\e[37;1m%s\e[0m", token.c_str());
        } else if (is_type(token) || is_special_literal(token)) {
            printf("\e[34m%s\e[0m", token.c_str());
        } else if (is_cpp(token)) {
            printf("\e[34;1m%s\e[0m", token.c_str());
        } else {
            printf("%s", token.c_str());
        }
    }
}

bool StringBuffer::draw(int x0, int y0, int x1, int y1) {
    if (!marked_for_update()) return false;
    int max_line = y1 - y0;
    int cur_line = 0;

    clear(ClearOpt::Screen);
    while (cur_line < _lines.size() && cur_line < max_line) {
        set_cursor_position(y0 + cur_line, x0);
        print_highlighted(_lines[cur_line]);
        fflush(stdout);
        // NOTE: Moves cursor AND flushes `line`:
        cur_line++;
    }

    mark_updated();
    return true;
}

int StringBuffer::max_col(int line) {
    //assert(_lines[line].size() - 1 <= std::numeric_limits<int>::max() || (printf("line = %d, max= %d\n", line, _lines.size()) && false));
    return std::min((int)_lines[line].size(), active_frame()._cols - 1);
}

int StringBuffer::min_col(int line) {
    return 0;
}

int StringBuffer::max_row() {
    //assert(_lines.size() - 1 <= std::numeric_limits<int>::max());
    return std::min((int)_lines.size() - 1, active_frame()._rows - 1);
}

int StringBuffer::min_row() {
    return 0;
}
