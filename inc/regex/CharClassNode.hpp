#ifndef CHAR_CLASS_NODE
# define CHAR_CLASS_NODE

# include "RegexNode.hpp"

class CharClassNode : public RegexNode {
    private:
    std::string pattern;
    bool        negated;

    public:
    CharClassNode(const std::string& pattern, bool negated);
    const std::string& getPattern() const;
    bool isNegated() const;
};

#endif