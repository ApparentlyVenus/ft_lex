#ifndef NFA_HPP
# define NFA_HPP

# include "../regex/RegexNode.hpp"
# include <vector>
# include <queue>
# include "../parser/Parser.hpp"

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