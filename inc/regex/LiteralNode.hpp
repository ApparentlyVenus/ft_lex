#ifndef LITERAL_NODE_HPP
# define LITERAL_NODE_HPP

# include "RegexNode.hpp"

class LiteralNode : public RegexNode {
    private:
    char ch;

    public:
    LiteralNode(char c);
    char getChar() const;
};


#endif