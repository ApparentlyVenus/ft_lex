#ifndef REGEX_NODE_HPP
# define REGEX_NODE_HPP

# include <string>

class RegexNode {
    public:
    virtual ~RegexNode() = default;
};

class LiteralNode : public RegexNode {
    private:
    char ch;

    public:
    LiteralNode(char c);
    char getChar() const;
};

class CharClassNode : public RegexNode {
    private:
    std::string pattern;
    bool        negated;

    public:
    CharClassNode(const std::string& pattern, bool negated);
    const std::string& getPattern() const;
    bool isNegated() const;
};

class DotNode : public RegexNode {};

class UnaryNode : public RegexNode {
    protected:
    RegexNode *child;

    public:
    UnaryNode(RegexNode* c);
    ~UnaryNode();

    RegexNode *getChild() const;
};

class BinaryNode : public RegexNode {
    protected:
    RegexNode *left;
    RegexNode *right;

    public:
    BinaryNode(RegexNode* l, RegexNode *r);
    ~BinaryNode();

    RegexNode *getLeft() const;
    RegexNode *getRight() const;
};

class StarNode : public UnaryNode {
    public:
    StarNode(RegexNode* c);
};
class PlusNode : public UnaryNode {
    public:
    PlusNode(RegexNode* c);
};
class QuestionNode : public UnaryNode {
    public:
    QuestionNode(RegexNode* c);
};

class ConcatNode : public BinaryNode {
    public:
    ConcatNode(RegexNode* l, RegexNode* r);
};
class AltNode : public BinaryNode {
    public:
    AltNode(RegexNode* l, RegexNode* r);
};


#endif