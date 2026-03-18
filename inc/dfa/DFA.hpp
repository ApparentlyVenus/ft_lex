#ifndef DFA_HPP
# define DFA_HPP

# include <map>
# include <set>
# include <queue>
# include "../nfa/NFA.hpp"

struct DFAState {
    int id;
    std::map<char, DFAState*> transitions;
    bool accepting;
    int ruleNumber;

    std::set<NFAState*> nfaStates;

    DFAState(int id, const std::set<NFAState*>& nfaStates);
};

class DFA {
    private:
    
    DFAState* start;
    std::set<DFAState*> states;

    public:
    
    static DFA fromNFA(const NFA& nfa);
    static std::set<NFAState*> epsilonClosure(NFAState* start);
};

#endif