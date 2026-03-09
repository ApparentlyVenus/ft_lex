#include "../../inc/regex/QuestionNode.hpp"

QuestionNode::QuestionNode(RegexNode *c) : UnaryNode(c) {}

NFAFragment QuestionNode::toNFA(int& stateCounter) {
    NFAFragment child = this->child->toNFA(stateCounter);

    NFAState *start = new NFAState(stateCounter++);
    NFAState *end = new NFAState(stateCounter++);

    start->epsilonTransitions.insert(child.start);
    start->epsilonTransitions.insert(end);
    child.end->epsilonTransitions.insert(end);

    return NFAFragment(start, end);
}