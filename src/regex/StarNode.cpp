#include "../../inc/regex/StarNode.hpp"

StarNode::StarNode(RegexNode *c) : UnaryNode(c) {}

NFAFragment StarNode::toNFA(int& stateCounter) {
    NFAFragment child = this->child->toNFA(stateCounter);

    NFAState *start = new NFAState(stateCounter++);
    NFAState *end = new NFAState(stateCounter++);

    start->epsilonTransitions.insert(end);
    start->epsilonTransitions.insert(child.start);
    child.end->epsilonTransitions.insert(end);
    child.end->epsilonTransitions.insert(child.start);

    return NFAFragment(start, end);
}