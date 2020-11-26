#include <iostream>
#include <string>
#include <fstream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <csignal>
#include <vector>
#include <sstream>
#include <termios.h>
#include <memory>
#include <algorithm>
#include <locale>

#include "term.hh"
#include "frame.hh"
#include "buffer.hh"
#include "rope.hh"
#include "arena.hh"

void resize_handler(int) {
    active_frame().update_size();
}

std::string open(std::string path) {
    std::ifstream ifs;
    ifs.open(path, std::ios::in);
    return std::string(std::istreambuf_iterator<char>(ifs),
                       std::istreambuf_iterator<char>{});
}

bool tick() {
    Frame& frame = active_frame();
    if (frame.is_marked_for_update()) {
        show_cursor(false);
        for (auto &b : frame.buffers) {
            b->draw(0, 0, frame._cols, frame._rows);
        }
        frame.restore_cursor_position();
        show_cursor(true);
    }

    Buffer& active_buffer = active_frame().active_buffer();
    Key key = read_key(STDIN_FILENO);
    switch (key) {
    case Key::CTRL_C: return false;
    case Key::ARROW_RIGHT: active_buffer.cursor_right(); break;
    case Key::ARROW_LEFT: active_buffer.cursor_left(); break;
    case Key::ARROW_UP: active_buffer.cursor_up(); break;
    case Key::ARROW_DOWN: active_buffer.cursor_down(); break;
    case Key::BACKSPACE: active_buffer.delete_backward(); break;
    case Key::DEL_KEY: active_buffer.delete_forward(); break;
    case Key::CTRL_K: active_buffer.kill_line(); break;
    case Key::CTRL_A: active_buffer.beginning_of_line(); break;
    case Key::CTRL_E: active_buffer.end_of_line(); break;
    case Key::ENTER: active_buffer.new_line(); break;
    case Key::KEY_NULL: break;
    default:
        if (std::isprint((char) key)) {
            active_buffer.insert((char) key);
        } else {
            // TODO: Handle unknown key.
        }
    }


    active_frame().restore_cursor_position();

    return true;
}

int main(int argc, char* argv[]) {
    Arena<RopeNode> arena(60);

    auto* a = make_rope("hello_");
    auto* b = make_rope("my_");
    auto* c = make_rope("na");
    auto* d = make_rope("me_i");
    auto* e = make_rope("s");
    auto* f = make_rope("_simon");

    auto* root = a; //->concat(b)->concat(c)->concat(d)->concat(e)->concat(f);
    // root = insert(root, "hello, world", 3);
    // std::cout << render(root) << std::endl;

    root = root->insert("anus", 3);
    root->dump();

    for (char c : *root) {
        std::cout << (int)c << "(" << c << ") ";
    }

    return 0;
    signal(SIGWINCH, resize_handler);

    if (argc != 2) { return 1; }
    active_frame().buffers.push_back(std::make_unique<StringBuffer>(open(argv[1])));

    active_frame().init();
    while (true) {
        if (!tick()) break;
    }
    clear(ClearOpt::Screen);
    set_cursor_position(0, 0);
    return 0;
}
