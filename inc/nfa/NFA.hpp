#ifndef NFA_HPP
# define NFA_HPP

# include "../parser/Parser.hpp"
# include "../regex/RegexNode.hpp"
# include <vector>
# include <queue>

class NFA {
    private:

    NFAState* start;
    std::set<NFAState*> states;

    public:

    NFA(NFAState* start);
    ~NFA();

    NFAState* getStart() const;
    std::set<NFAState*> getStates() const;

    static NFA combineRules(const std::vector<Rule>& rules);
    
};

#endif