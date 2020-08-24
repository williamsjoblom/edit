#pragma once

#include <cstring>
#include <utility>

struct RopeNode {
    int weight;
    // NOTE: Not nessecarily null terminated (but of length `weight`):
    const char* string;
    RopeNode *left;
    RopeNode *right;

    // Leaf constructor (null terminated string).
    RopeNode(const char *s)
        : weight{std::strlen(s)}, string{s}, left{nullptr}, right{nullptr} {}
    // Leaf constructor (not null terminated).
    RopeNode(const char *s, int length)
        : weight{std::strlen(s)}, string{s}, left{nullptr}, right{nullptr} {}
    // Parent constructor.
    RopeNode(RopeNode *lhs, RopeNode *rhs)
        : weight{lhs->weight}, string{nullptr}, left{lhs}, right{rhs} {}
};

RopeNode* make_rope(const char* string){
    return new RopeNode{string};
};

std::pair<RopeNode*, int> node_at(RopeNode* node, int index) {
    if (node->weight <= index && node->right != nullptr) {
        return node_at(node->right, index - node->weight);
    } else if (node->left != nullptr) {
        return node_at(node->left, index);
    } else {
        return {node, index};
    }
}

char char_at(RopeNode *node, int index) {
    auto [n, i] = node_at(node, index);
    return n->string[i];
}

RopeNode* concat(RopeNode *left, RopeNode *right) {
    return new RopeNode{left, right};
}

std::pair<RopeNode*, RopeNode*> split(RopeNode* root, int index) {
    auto [n, i] = node_at(root, index);
    if (i < n->weight - 1) {
        // Index within a node, let's split it!
        RopeNode* lhs = new RopeNode(n->string, i);
        RopeNode* rhs = new RopeNode(&n->string[i], n->weight - i);
        *n = RopeNode(lhs, rhs);
    }
}

RopeNode *insert(RopeNode* root, const char* string, int index) {
    auto [left, right] = split(root, index);

}

std::
