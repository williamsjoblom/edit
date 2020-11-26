#pragma once

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class RopeLeafIterator;
class RopeCharIterator;

class RopeNode {
private:
    // NOTE: Not nessecarily null terminated (but of length `weight`):
    const char *string;

    int calculate_weight();
public:
    std::size_t weight;
    RopeNode *left;
    RopeNode *right;

    // Leaf constructor (null terminated string).
    RopeNode(const char *s)
        : weight{std::strlen(s)}, string{s}, left{nullptr}, right{nullptr} {}
    // Leaf constructor (not null terminated).
    RopeNode(const char *s, std::size_t length)
        : weight{length}, string{s}, left{nullptr}, right{nullptr} {}

    // Parent constructor.
    RopeNode(RopeNode *lhs, RopeNode *rhs)
        : weight{lhs->weight}, string{nullptr}, left{lhs}, right{rhs} {}



    bool is_leaf() const { return string != nullptr; }
    bool is_parent() const { return !is_leaf(); }

    std::string str(bool accept_parent = false) const;

    void dump(int indent = 0) const;

    int reweigh();

    std::pair<const RopeNode &, int> node_at(int index) const;
    char operator[](int index) const;

    RopeNode *concat(RopeNode *other);


    std::pair<RopeNode *, RopeNode *> split0(RopeNode *orphan, int index) {
        if (index == weight) {
            return is_leaf()
                ? std::make_pair(this, nullptr)
                : std::make_pair(left, right);
        } else if (index > weight) {
            auto [new_right, new_orphan] = right->split0(orphan, index - weight);
            return std::make_pair(left->concat(new_right), new_orphan);
        } else {
            if (is_parent()) {
                auto [new_left, new_orphan] = left->split0(orphan, index);
                return std::make_pair(new_left, new_orphan->concat(right));
            } else {
                RopeNode *left = index > 0 ? new RopeNode(string, index) : nullptr;
                RopeNode *right = new RopeNode(&string[index], weight - index);
                return std::make_pair(left, right);
            }
        }
    }

    /**
     * Split a rope at the given index, returning a pair of ropes
     * representing the right- and left-hand-side after the split.
     */
    std::pair<RopeNode *, RopeNode *> split(int index) {
        return this->split0(nullptr, index);
    }

    /**
     * Insert the given string into the rope at the given index.
     */
    RopeNode *insert(const char *string, int index);

    RopeNode *kill(int start, int length);

    void render0(std::stringstream &ss) {
        if (is_leaf()) {
            ss << std::string_view(string, weight);
        } else {
            if (left)
                left->render0(ss);
            if (right)
                right->render0(ss);
        }
    }

    const std::string render() {
        std::stringstream ss;
        render0(ss);
        return ss.str();
    }

    int node_count() {
        if (is_leaf())
            return 1;
        else
            return left->node_count() + right->node_count() + 1;
    }

    RopeCharIterator begin();
    RopeCharIterator end();

    RopeLeafIterator begin_leaf();
    RopeLeafIterator end_leaf();

    void *operator new(std::size_t);
    void operator delete(void*) {}
};

class RopeLeafIterator {
private:
    std::stack<RopeNode *> stack;
    RopeNode *next;
public:
    using difference_type = int;
    using value_type = RopeNode;
    using pointer = RopeNode *;
    using reference = RopeNode &;

    RopeLeafIterator(RopeNode *root = nullptr) {
        stack.push(root);
        next = next_leaf();
    }

    RopeNode *next_leaf() {
        while (!stack.empty()) {
            RopeNode *top = stack.top();
            stack.pop();
            if (top == nullptr) {
                return nullptr;
            }

            if (top->is_leaf()) {
                return top;
            }

            if (top->right != nullptr) {
                stack.push(top->right);
            }
            if (top->left != nullptr) {
                stack.push(top->left);
            }
        }

        return nullptr;
    }

    RopeLeafIterator &operator++() {
        next = next_leaf();
        return *this;
    }

    RopeLeafIterator operator++(int) {
        RopeLeafIterator result = *this;
        ++*this;
        return result;
    }

    bool operator!=(RopeLeafIterator &other) { return next != other.next; }

    RopeNode &operator*() { return *next; }
};

class RopeCharIterator {
private:
    RopeLeafIterator leaf_iter;
    int index;
public:
    using difference_type = int;
    using value_type = char;
    using pointer = char *;
    using reference = char &;

    RopeCharIterator(RopeLeafIterator leaf_iter)
        : leaf_iter{leaf_iter}, index{0} { }

    RopeCharIterator()
        : leaf_iter{RopeLeafIterator(nullptr)}, index{0} {}

    RopeCharIterator &operator++() {
        index++;
        if (index < (*leaf_iter).weight) {
            return *this;
        } else {
            index = 0;
            ++leaf_iter;
            return *this;
        }
    }

    RopeCharIterator operator++(int) {
        RopeCharIterator result = *this;
        ++*this;
        return result;
    }

    bool operator!=(RopeCharIterator &other) {
        return leaf_iter != other.leaf_iter ||
            index != other.index;
    }

    char operator*() { return (*leaf_iter).str()[index]; }
};

RopeNode *make_rope(const char *string);
