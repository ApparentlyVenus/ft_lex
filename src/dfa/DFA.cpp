#include "../../inc/dfa/DFA.hpp"

DFAState::DFAState(int id, const std::set<NFAState*>& nfaStates)
    :   id(id), accepting(false), ruleNumber(-1), nfaStates(nfaStates) {

    for (NFAState *state :  nfaStates) {
        if (state->accepting) {
            accepting = true;
            if (ruleNumber == -1 || state->ruleNumber < ruleNumber)
                ruleNumber = state->ruleNumber;
        }
    }
}

DFA DFA::fromNFA(const NFA& nfa) {
    
}