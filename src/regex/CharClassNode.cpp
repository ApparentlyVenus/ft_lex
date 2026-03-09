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
            i += 2;
        }
        else {
            result.insert(pattern[i]);
        }
    }

    if (isNegated()) {
        std::set<char> temp = result;
        result.clear();
        for (char c = 0; c < 256; c++) {
            if (c != '\n' && temp.find(c) == temp.end())
                result.insert(c);
        }
    }
    return result;
}

NFAFragment CharClassNode::toNFA(int& stateCounter) {
    NFAState *start = new NFAState(stateCounter++);
    NFAState *end = new NFAState(stateCounter++);

    std::set<char> chars = expandCharClass(pattern);

    for (char c : chars)
        start->transitions[c].insert(end);

    return NFAFragment(start, end);
}