#include "../../inc/regex/RegexNode.hpp"

NFAState::NFAState(int id) : id(id), accepting(false), ruleNumber(-1) {}

NFAFragment::NFAFragment(NFAState *start, NFAState *end) : start(start), end(end) {}

UnaryNode::UnaryNode(RegexNode *c) : child(c) {}

UnaryNode::~UnaryNode() {
    delete child;
}

RegexNode *UnaryNode::getChild() const {
    return child;
}

BinaryNode::BinaryNode(RegexNode *l, RegexNode *r) : left(l), right(r) {}

BinaryNode::~BinaryNode() {
    delete left;
    delete right;
}

RegexNode *BinaryNode::getLeft() const {
    return left;
}

RegexNode *BinaryNode::getRight() const {
    return right;
}







