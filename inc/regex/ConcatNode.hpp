#ifndef CONCAT_NODE_HPP
# define CONCAT_NODE_HPP

# include "RegexNode.hpp"

class ConcatNode : public BinaryNode {
    public:
    ConcatNode(RegexNode* l, RegexNode* r);
    NFAFragment toNFA(int& stateCounter) override;
};

#endif