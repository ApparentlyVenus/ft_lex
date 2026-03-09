#include "../../inc/regex/PlusNode.hpp"

PlusNode::PlusNode(RegexNode *c) : UnaryNode(c) {}

NFAFragment PlusNode::toNFA(int& stateCounter) {
    NFAFragment child = this->child->toNFA(stateCounter);
    
    NFAState *start = new NFAState(stateCounter++);
    NFAState *end = new NFAState(stateCounter++);

    start->epsilonTransitions.insert(child.start);
    child.end->epsilonTransitions.insert(child.start);
    child.end->epsilonTransitions.insert(end);

    return NFAFragment(start, end);

}
