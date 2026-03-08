#ifndef REGEX_NODE_HPP
# define REGEX_NODE_HPP

# include <string>
# include <map>
# include <set>

struct NFAState {
    int id;
    std::map<char, std::set<NFAState*>> transitions;
    std::set<NFAState*> epsilonTransitions;
    bool accepting;
    int ruleNumber;

    NFAState(int id);
};

struct NFAFragment {
    NFAState *start;
    NFAState *end;
};

class RegexNode {
    public:
    virtual ~RegexNode() = default;
    virtual NFAFragment toNFA(int& stateCounter) = 0;
};

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

#endif