#include "../../inc/regex/CharClassNode.hpp"

CharClassNode::CharClassNode(const std::string& pattern, bool negated)
    : pattern(pattern), negated(negated) {}

const std::string& CharClassNode::getPattern() const {
    return pattern;
}

bool CharClassNode::isNegated() const {
    return negated;
}