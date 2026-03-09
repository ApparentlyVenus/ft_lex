#include "../../inc/regex/CharClassNode.hpp"

CharClassNode::CharClassNode(const std::string& pattern, bool negated)
    : pattern(pattern), negated(negated) {}

const std::string& CharClassNode::getPattern() const {
    return pattern;
}

bool CharClassNode::isNegated() const {
    return negated;
}

std::set<char> CharClassNode::expandCharClass(const std::string& pattern) {
    std::set<char> result;

    for (size_t i = 0; i < pattern.length(); i++) {
        if (i + 2 < pattern.length() && pattern[i + 1] == '-') {
            for (char c = pattern[i]; c <= pattern[i + 2]; c++)
                result.insert(c);
        }
        else {
            result.insert(pattern[i]);
        }
    }
    return result;
}