#ifndef ALT_NODE_HPP
# define ALT_NODE_HPP

# include "RegexNode.hpp"

class AltNode : public BinaryNode {
    public:
    AltNode(RegexNode* l, RegexNode* r);
    NFAFragment toNFA(int& stateCounter) override;
};

#endif