#include "rope.hh"
#include "arena.hh"

int RopeNode::calculate_weight() {
    if (is_parent()) {
        return left->weight + right->calculate_weight();
    } else {
        return weight;
    }
}

std::string RopeNode::str(bool accept_parent) const {
    if (is_leaf())
        return std::string{string, weight};
    else if (accept_parent)
        return left->str() + right->str();
    else
        assert(false);
}

void RopeNode::dump(int indent) const {
    std::cout << std::string(2 * indent, ' ') << '(' << this->weight << ')';
    if (this->is_leaf())
        std::cout << ": " << this->str();
    std::cout << std::endl;
    if (this->is_parent()) {
        if (this->left)
            left->dump(indent + 1);
        if (this->right)
            right->dump(indent + 1);
    }
}

int RopeNode::reweigh() {
    if (is_parent()) {
        weight = left->reweigh();
        return weight + right->reweigh();
    } else {
        return weight;
    }
}

std::pair<const RopeNode &, int> RopeNode::node_at(int index) const {
    if (weight <= index && right != nullptr) {
        return right->node_at(index - weight);
    } else if (left != nullptr) {
        return left->node_at(index);
    } else {
        return {*this, index};
    }
}

char RopeNode::operator[](int index) const {
    auto [n, i] = node_at(index);
    return n.string[i];
}

RopeNode *RopeNode::concat(RopeNode *other) {
    auto *result = new RopeNode{this, other};
    result->reweigh();
    return result;
}

RopeNode *RopeNode::insert(const char *string, int index) {
    auto [left, right] = split(index);
    if (left == nullptr && right == nullptr)
        return new RopeNode(string);
    else if (left == nullptr && right != nullptr)
        return (new RopeNode(string))->concat(right);
    else if (left != nullptr && right == nullptr)
        return left->concat(new RopeNode(string));
    else
        return left->concat((new RopeNode(string))->concat(right));
}

RopeNode *RopeNode::kill(int start, int length) {
    RopeNode *left = split(start).first;
    RopeNode *right = split(start + length).second;
    if (left == nullptr && right == nullptr)
        return new RopeNode("");
    else if (left == nullptr && right != nullptr)
        return right;
    else if (left != nullptr && right == nullptr)
        return left;
    else
        return left->concat(right);
}


RopeCharIterator RopeNode::begin() {
    return RopeCharIterator(begin_leaf());
}

RopeCharIterator RopeNode::end() {
    return RopeCharIterator();
}

RopeLeafIterator RopeNode::begin_leaf() {
    return RopeLeafIterator(this);
}

RopeLeafIterator RopeNode::end_leaf() {
    return RopeLeafIterator();
}

// template class Arena<RopeNode>;
// Arena<RopeNode>* Arena<RopeNode>::current = nullptr;
void *RopeNode::operator new(std::size_t) {
    return Arena<RopeNode>::current->alloc();
}

RopeNode *make_rope(const char *string) { return new RopeNode{string}; }
