#pragma once

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>

#include "term.hh"

class Buffer {
public:
    int _col = 0, _row = 0;
private:
    bool _update = true;
public:
    void mark_for_update() { _update = true; }
    bool marked_for_update() { return _update; }
    void mark_updated() { assert(_update); _update = false; }

    /**
     * Draw buffer, returns true if the buffer was redrawn.
     */
    virtual bool draw(int x0, int y0, int x1, int y1) = 0;

    virtual int max_col(int row) = 0;
    virtual int min_col(int row) = 0;
    virtual int max_row() = 0;
    virtual int min_row() = 0;

    void clamp_cursor() {
        _row = std::min(max_row(), std::max(min_row(), _row));
        _col = std::min(max_col(_row), std::max(min_col(_row), _col));
    }

    void cursor_up();
    void cursor_down();
    void cursor_left();
    void cursor_right();

    virtual void insert(char c) { }
    virtual void beginning_of_line() { }
    virtual void end_of_line() { }
    virtual void new_line() { }
    virtual void delete_backward() { }
    virtual void delete_forward() { }
    virtual void kill_line() { }
};


class StringBuffer : public Buffer {
public:
    std::vector<std::string> _lines;

    StringBuffer(std::string s) {
        std::istringstream iss{s};
        std::string line;
        while (std::getline(iss, line))
            _lines.push_back(line);
    }

    bool draw(int x0, int y0, int x1, int y1) override;

    int max_col(int line) override;
    int min_col(int line) override;
    int max_row() override;
    int min_row() override;

    std::string& previous_line() {
        assert(_row - 1 >= 0);
        return _lines[_row - 1];
    }
    std::string& current_line() { return _lines[_row]; }
    std::string& next_line() {
        assert(_row + 1 < _lines.size());
        return _lines[_row + 1];
    }

    void insert(char c) override {
        current_line().insert(_col, 1, c);
        _col++;
        mark_for_update();
    }

    void beginning_of_line() override {
        _col = min_col(_row);
    }

    void end_of_line() override {
        _col = max_col(_row);
    }

    void new_line() override {
        std::string next = current_line().substr(0, _col);
        std::string curr = current_line().substr(_col);
        current_line() = curr;
        _lines.insert(_lines.begin() + _row, next);
        _row++;
        _col = 0;
        mark_for_update();
    }

    void delete_backward() override {
        if (_col > 0) {
            current_line().erase(_col - 1, 1);
            _col--;
            mark_for_update();
        } else if (_row > 0) {
            _row--;
            end_of_line();

            current_line() = current_line() + next_line();
            _lines.erase(_lines.begin() + _row + 1);
            mark_for_update();
        } else {
            // NOTE: Beginning of file.
        }
    }

    void delete_forward() override {
        std::string& line = current_line();
        if (_col < line.size()) {
            line.erase(_col, 1);
            mark_for_update();
        } else if (_row < _lines.size()) {
            current_line() = current_line() + next_line();
            _lines.erase(_lines.begin() + _row + 1);
            mark_for_update();
        } else {
            // NOTE: End of file.
        }
    }

    void kill_line() override {
        if (current_line().size() > 0) {
            current_line() = current_line().substr(0, _col);
            mark_for_update();
        }
    }
};
