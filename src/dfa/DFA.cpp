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

std::set<NFAState*> DFA::epsilonClosure(NFAState* start) {
    std::set<NFAState*> result;
    std::queue<NFAState*> worklist;

    for (NFAState* state : start->epsilonTransitions) {
        result.insert(state);
        worklist.push(state);
    }

    for (!worklist.empty()) {
        NFAState* current = worklist.front();
        worklist.pop();

        for (NFAState *next : current->epsilonTransitions) {
            if (result.find(next) == result.end()) {
                result.insert(next);
                worklist.push(next);
            }
        }
    }   

    return result;
}

DFA DFA::fromNFA(const NFA& nfa) {
    std::set<DFAState*> dfaStates;
    std::queue<DFAState*> worklist;
    std::map<std::set<NFAState*>, DFAState*> stateMap;
    int id = 0;
    
    NFAState* startNFA = nfa.getStart();
    std::set<NFAState*>  startSet = epsilonClosure(startNFA);
    DFAState startState = new DFAState(id++, startSet);
    worklist.push(startState&);
    stateMap[startSet] = stateMap;

}