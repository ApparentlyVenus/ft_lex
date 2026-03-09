#include "../../inc/regex/AltNode.hpp"

AltNode::AltNode(RegexNode *l, RegexNode *r) : BinaryNode(l, r) {}

NFAFragment AltNode::toNFA(int& stateCounter) {
    NFAFragment left = this->left->toNFA(stateCounter);
    NFAFragment right = this->right->toNFA(stateCounter);

    NFAState *start = new NFAState(stateCounter++);
    NFAState *end = new NFAState(stateCounter++);

    start->epsilonTransitions.insert(left.start);
    start->epsilonTransitions.insert(right.start);
    left.end->epsilonTransitions.insert(end);
    right.end->epsilonTransitions.insert(end);

    return NFAFragment(start, end);
}