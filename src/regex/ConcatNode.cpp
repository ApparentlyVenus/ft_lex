#include "../../inc/regex/ConcatNode.hpp"

ConcatNode::ConcatNode(RegexNode *l, RegexNode *r) : BinaryNode(l, r) {}

NFAFragment ConcatNode::toNFA(int &stateCounter) {
    NFAFragment left = this->left->toNFA(stateCounter);
    NFAFragment right = this->right->toNFA(stateCounter);
    left.end->epsilonTransitions.insert(right.start);
    return NFAFragment(left.start, right.end);
}