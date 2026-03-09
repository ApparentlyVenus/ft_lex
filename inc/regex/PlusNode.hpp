#ifndef PLUS_NODE_HPP
# define PLUS_NODE_HPP

# include "RegexNode.hpp"

class PlusNode : public UnaryNode {
    public:
    PlusNode(RegexNode* c);
    NFAFragment toNFA(int& stateCounter) override;
};

#endif