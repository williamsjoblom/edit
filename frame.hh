#pragma once

#include <vector>
#include <memory>

#include "buffer.hh"
#include "term.hh"

class Buffer;
class Frame;

Frame &active_frame();

class Frame {
public:
    int _rows;
    int _cols;

    std::vector<std::unique_ptr<Buffer>> buffers;

    Buffer& active_buffer() {
        return *buffers[0];
    }

    void mark_for_update() {
        for (auto &buffer : buffers) {
            buffer->mark_for_update();
        }
    }

    bool is_marked_for_update() {
        return std::any_of(buffers.begin(), buffers.end(), [](auto& b){
            return b->marked_for_update();
        });
    }

    void restore_cursor_position() {
        Buffer& buffer = active_buffer();
        set_cursor_position(buffer._row, buffer._col);
    }

    void update_size() {
        auto [rows, cols] = get_term_size();

        bool size_changed = _cols != cols || _rows != rows;
        _cols = cols; _rows = rows;
        if (size_changed) {
            //clear(ClearOpt::Screen);
            restore_cursor_position();
            mark_for_update();
        }
    }

    void init() {
        update_size();
        enable_raw_mode();
    }


};
