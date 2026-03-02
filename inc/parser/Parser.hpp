#ifndef PARSER_HPP
# define PARSER_HPP

# include <vector>
# include <map>
# include "RegexNode.hpp"
# include "../tokenizer/Tokenizer.hpp"

struct Rule {
    RegexNode *pattern;
    std::string action;
    int priority;

    Rule(RegexNode *p, const std::string& action, int priority);
};

class Parser {
    private:
    std::vector<Token> _tokens;
    std::map<std::string, std::string> _definitions;
    size_t _pos;

    Token peek() const;
    Token advance();
    bool isAtEnd() const;

    void parseHeader();
    void expandReferences();
    std::string expandDefinitions(const std::string& pattern, const Token& token);

    RegexNode *parsePattern();
    RegexNode *parseAlt();
    RegexNode *parseConcat();
    RegexNode *parsePostfix();
    RegexNode *parsePrimary();

    std::vector<Rule> parseRules();
    void resolveActions(std::vector<Rule>& rules);

    public:
    Parser(const std::vector<Token>& tokens);
    
    std::vector<Rule> parse();
};

#endif