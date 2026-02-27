#include "../../inc/parser/RegexNode.hpp"

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

LiteralNode::LiteralNode(char c) : ch(c) {}

char LiteralNode::getChar() const {
    return ch;
}

CharClassNode::CharClassNode(const std::string& pattern, bool negated)
    : pattern(pattern), negated(negated) {}

const std::string& CharClassNode::getPattern() const {
    return pattern;
}

bool CharClassNode::isNegated() const {
    return negated;
}

StarNode::StarNode(RegexNode *c) : UnaryNode(c) {}

PlusNode::PlusNode(RegexNode *c) : UnaryNode(c) {}

QuestionNode::QuestionNode(RegexNode *c) : UnaryNode(c) {}

ConcatNode::ConcatNode(RegexNode *l, RegexNode *r) : BinaryNode(l, r) {}

AltNode::AltNode(RegexNode *l, RegexNode *r) : BinaryNode(l, r) {}
