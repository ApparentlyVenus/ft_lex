#include "../../inc/parser/Parser.hpp"

Rule::Rule(RegexNode *p, const std::string &action, int priority)
    : pattern(p), action(action), priority(priority) {}

Parser::Parser(const std::vector<Token> &tokens) : _tokens(tokens), _definitions(), _pos(0) {}

Token Parser::peek() const {
    return _tokens.at(_pos);
}

Token Parser::advance() {
    return _tokens.at(_pos++);
}

bool Parser::isAtEnd() const {
    return (_pos >= _tokens.size() || _tokens.at(_pos).type == TOK_EOF);
}

void Parser::parseHeader() {
    while (!isAtEnd()) {
        if (peek().type == TOK_PERCENT_PERCENT) {
            advance();
            return ;
        }

        if (peek().type == TOK_PERCENT_LBRACE) {
            while (peek().type != TOK_PERCENT_RBRACE)
                advance();
            advance();
            continue;
        }

        if (peek().type == TOK_DEFINITION) {
            Token defToken = advance();
            size_t space = defToken.value.find(' ');
            std::string name = defToken.value.substr(0, space);
            std::string pattern = defToken.value.substr(space + 1);
            _definitions[name] = expandDefinitions(pattern, defToken);
        }
    }
}

std::string Parser::expandDefinitions(const std::string& pattern, const Token& defToken) {
    std::string result;
    size_t i = 0;

    while (i < pattern.size()) {
        if (pattern[i] == '{') {
            size_t end = pattern.find('}', i);
            if (end == std::string::npos)
                throw std::runtime_error("unterminated { at " + 
                    std::to_string(defToken.line) + ":" + std::to_string(defToken.column));
            std::string name = pattern.substr(i + 1, end - i - 1);
            if (_definitions.find(name) == _definitions.end())
                throw std::runtime_error("undefined: " + name + " at " + 
                    std::to_string(defToken.line) + ":" + std::to_string(defToken.column));
            result += expandDefinitions(_definitions[name], defToken);
            i = end + 1;
        } else {
            result += pattern[i];
            i++;
        }
    }
    return result;
}

void Parser::expandReferences() {
    for (size_t i = 0; i < _tokens.size(); i++) {
        if (_tokens[i].type == TOK_DEFINITION_REFERENCE) {
            std::string name = _tokens[i].value;
            if (_definitions.find(name) == _definitions.end())
                throw std::runtime_error("undefined reference: " + name + " at " + 
                    std::to_string(_tokens[i].line) + ":" + std::to_string(_tokens[i].column));
            
            std::string pattern = _definitions[name];
            Tokenizer t(pattern);
            std::vector<Token> expanded = t.tokenizePatternExtern();
            
            if (!expanded.empty() && expanded.back().type == TOK_EOF)
                expanded.pop_back();
            
            _tokens.erase(_tokens.begin() + i);
            _tokens.insert(_tokens.begin() + i, expanded.begin(), expanded.end());
            i += expanded.size() - 1;
        }
    }
}

RegexNode *Parser::parsePattern() {
    return parseAlt();
}

RegexNode *Parser::parseAlt() {
    RegexNode *left = parseConcat();
    while (!isAtEnd() && peek().type == TOK_PIPE) {
        if (_pos + 1 < _tokens.size() && _tokens[_pos + 1].type == TOK_NEWLINE)
            return left;
        advance();
        RegexNode *right = parseConcat();
        left = new AltNode(left, right);
    }
    return left;
}

RegexNode *Parser::parseConcat() {
    RegexNode *left = parsePostfix();
    TokenType type = peek().type;
    while (type == TOK_LITERAL || type == TOK_LBRACE 
        || type == TOK_LBRACKET || type == TOK_LPAREN
        || type == TOK_DEFINITION_REFERENCE || type == TOK_DOT 
        || type == TOK_STRING || type == TOK_CHARCLASS) {
        RegexNode *right = parsePostfix();
        left = new ConcatNode(left, right);
        type = peek().type;
    }
    return left;
}

RegexNode *Parser::parsePostfix() {
    RegexNode *node = parsePrimary();
    TokenType type = peek().type;
    switch (type) {
        case (TOK_STAR):
            advance();
            return new StarNode(node);
        case (TOK_PLUS):
            advance();
            return new PlusNode(node);
        case (TOK_QUESTION):
            advance();
            return new QuestionNode(node);
        default:
            return node;
    }
    return node;
}

RegexNode *Parser::parsePrimary() {
    Token token = advance();

    switch (token.type) {
        case (TOK_LITERAL): {
            if (token.value.empty())
                throw std::runtime_error("empty literal at " + 
                    std::to_string(token.line) + ":" + std::to_string(token.column));
            return new LiteralNode(token.value[0]);
        }
        case (TOK_STRING): {
            if (token.value.empty())
                throw std::runtime_error("empty string at " + 
                    std::to_string(token.line) + ":" + std::to_string(token.column));
            RegexNode *result = new LiteralNode(token.value[0]);
            for (size_t i = 1; i < token.value.length(); i++)
                result = new ConcatNode(result, new LiteralNode(token.value[i]));
            return result;
        }
        case (TOK_CHARCLASS): {
            if (token.value.empty())
                throw std::runtime_error("empty charclass at " + 
                    std::to_string(token.line) + ":" + std::to_string(token.column));
            bool negated = (token.value[0] == '^');
            std::string pattern;
            if (negated)
                pattern = token.value.substr(1);
            else
                pattern = token.value;
            return new CharClassNode(pattern, negated);
        }
        case (TOK_LPAREN): {
            RegexNode *paren = parsePattern();
            if (peek().type != TOK_RPAREN)
                throw std::runtime_error("expected ) at " + std::to_string(token.line) + ":" + std::to_string(token.column));
            advance();
            return paren;
        }
        case (TOK_DOT):
            return new DotNode();
        
        default:
            throw std::runtime_error("unexpected token at " + std::to_string(token.line) + ":" + std::to_string(token.column));
    }            
}

/* 
pattern     → alternation
alternation → concat ('|' concat)*
concat      → postfix postfix*
postfix     → primary ('*' | '+' | '?')?
primary     → LITERAL | STRING | CHARCLASS | '(' pattern ')' | '{' NAME '}'
*/

std::vector<Rule> Parser::parseRules() {
    std::vector<Rule> rules;
    int priority = 0;

    while (!isAtEnd()) {
        while (peek().type == TOK_NEWLINE)
            advance();
        if (isAtEnd())
            break ;

        if (peek().type == TOK_PERCENT_PERCENT) {
            advance();
            break ;
        }

        RegexNode *pattern = parsePattern();
        
        if (peek().type == TOK_PIPE) {
            std::string action = advance().value;  // "|"
            rules.push_back(Rule(pattern, action, priority++));
        } else if (peek().type == TOK_C_CODE) {
            std::string action = advance().value;
            rules.push_back(Rule(pattern, action, priority++));
        } else {
            throw std::runtime_error("expected action at " + 
                std::to_string(peek().line) + ":" + std::to_string(peek().column));
        }            
        if (peek().type == TOK_NEWLINE)
            advance();
    }
    return rules;
}

void Parser::resolveActions(std::vector<Rule>& rules) {
    for (size_t i = 0; i < rules.size(); i++) {
        if (rules[i].action == "|") {
            size_t j = i + 1;
            while (j < rules.size() && rules[j].action == "|")
                j++;
            if (j >= rules.size())
                throw  std::runtime_error("pipe continuation with no action");
            std::string action = rules[j].action;
            for (size_t k = i; k < j; k++) {
                rules[k].action = action;
            }
        }
    }
}

std::vector<Rule> Parser::parse() {
    parseHeader();
    expandReferences();
    std::vector<Rule> rules = parseRules();
    resolveActions(rules);
    return rules;
}
