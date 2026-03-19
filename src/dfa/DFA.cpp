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

DFA::DFA(DFAState* start, const std::set<DFAState*>& states)
    : start(start), states(states) {}

DFA::~DFA() {
    for (DFAState* state : states)
        delete state;
}

DFAState *DFA::getStart() const {
    return start;
}

std::set<DFAState*> DFA::getStates() const {
    return states;
}

std::set<NFAState*> DFA::epsilonClosure(const std::set<NFAState*>& states) {
    std::set<NFAState*> result;
    std::queue<NFAState*> worklist;

    for (NFAState* state : states) {
        result.insert(state);
        worklist.push(state);
    }

    while (!worklist.empty()) {
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

    std::set<NFAState*> startStates = epsilonClosure({nfa.getStart()}); 
    DFAState *start = new DFAState(id++, startStates);
    worklist.push(start);
    dfaStates.insert(start);
    stateMap[startStates] = start;

    while (!worklist.empty()) {
        DFAState *current = worklist.front();
        worklist.pop();

        for (int c = 0; c < 256; c++) {
            std::set<NFAState *> reachable;
            for (NFAState * nfaState : current->nfaStates) {
                auto it = nfaState->transitions.find((char)c);
                if (it != nfaState->transitions.end())
                    reachable.insert(it->second.begin(), it->second.end());
            }

            if (reachable.empty())
                continue ;

            std::set<NFAState *> closure = epsilonClosure(reachable);
            DFAState *next;

            if (stateMap.find(closure) == stateMap.end()) {
                next = new DFAState(id++, closure);
                stateMap[closure] = next;
                dfaStates.insert(next);
                worklist.push(next);
            } else {
                next = stateMap[closure];
            }
            current->transitions[(char)c] = next;
        }
    }
    return DFA(start, dfaStates);
}