#include "../../inc/regex/LiteralNode.hpp"

LiteralNode::LiteralNode(char c) : ch(c) {}

char LiteralNode::getChar() const {
    return ch;
}

NFAFragment LiteralNode::toNFA(int& stateCounter) {
    NFAState *start = new NFAState(stateCounter++);
    NFAState *end = new NFAState(stateCounter++);
    start->transitions[ch].insert(end);
    return NFAFragment(start, end);
}