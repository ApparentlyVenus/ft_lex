#include "../../inc/regex/DotNode.hpp"

NFAFragment DotNode::toNFA(int& stateCounter) {
    NFAState* start = new NFAState(stateCounter++);
    NFAState* end = new NFAState(stateCounter++);
    
    for (char c = 0; c < 256; c++) {
        if (c != '\n')
            start->transitions[c].insert(end);
    }
    
    return {start, end};
}