#include "../../inc/regex/LiteralNode.hpp"

LiteralNode::LiteralNode(char c) : ch(c) {}

char LiteralNode::getChar() const {
    return ch;
}