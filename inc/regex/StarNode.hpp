#ifndef STAR_NODE_HPP
# define STAR_NODE_HPP

# include "RegexNode.hpp"

class StarNode : public UnaryNode {
    public:
    StarNode(RegexNode* c);
    NFAFragment toNFA(int& stateCounter) override;
};

#endif