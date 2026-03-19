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
    
    DFA(DFAState* start, const std::set<DFAState*>& states);
    ~DFA();

    DFAState *getStart() const;
    std::set<DFAState*> getStates() const;
    
    static DFA fromNFA(const NFA& nfa);
    static std::set<NFAState*> epsilonClosure(const std::set<NFAState*>& states);
};

#endif