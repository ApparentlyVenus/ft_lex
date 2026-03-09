#include "../../inc/nfa/NFA.hpp"

NFA::NFA(NFAState* start) : start(start) {
    std::set<NFAState*> visited;
    std::queue<NFAState*> queue;
    queue.push(start);

    while (!queue.empty()) {
        NFAState* current = queue.front();
        queue.pop();

        if (visited.find(current) != visited.end())
            continue ;

        visited.insert(current);
        states.insert(current);

        for (auto& transition : current->transitions) {
            for (NFAState* next : transition.second)
                queue.push(next);
        }

        for (NFAState* next : current->epsilonTransitions)
            queue.push(next);
    }
}

NFA::~NFA() {
    for (NFAState* state : states)
        delete state;
}

NFAState *NFA::getStart() const {
    return start;
}

std::set<NFAState*> NFA::getStates() const {
    return states;
}

NFA NFA::combineRules(const std::vector<Rule>& rules) {
    int stateCounter = 0;
    NFAState* globalStart = new NFAState(stateCounter++);

    for (const Rule& rule : rules) {
        NFAFragment frag = rule.pattern->toNFA(stateCounter);

        frag.end->accepting = true;
        frag.end->ruleNumber = rule.priority;

        globalStart->epsilonTransitions.insert(frag.start);
    }
    
    return NFA(globalStart);
}

