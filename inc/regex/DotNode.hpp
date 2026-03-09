#ifndef DOT_NODE_HPP
# define DOT_NODE_HPP

# include "RegexNode.hpp"

class DotNode : public RegexNode {
    NFAFragment toNFA(int& stateCounter) override;
};

#endif