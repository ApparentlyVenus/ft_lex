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
            std::string def = advance().value;
            size_t space = def.find(' ');
            std::string name = def.substr(0, space);
            std::string pattern = def.substr(space + 1);
            _definitions[name] = pattern;
        }
    }
}

void Parser::expandDefinitions() {
    for (size_t i = 0; i < _tokens.size(); i++) {
        if (_tokens[i].type == TOK_DEFINITION_REFERENCE) {
            if (_definitions.find(_tokens[i].value) == _definitions.end())
                throw std::runtime_error("undefined expression at " + std::to_string(_tokens[i].line) + ":" + std::to_string(_tokens[i].column));
            std::string pattern = _definitions[_tokens[i].value];
            Tokenizer defTokenizer(pattern);
            std::vector<Token> expanded = defTokenizer.tokenize();
            _tokens.erase(_tokens.begin() + i);
            _tokens.insert(_tokens.begin() + i, expanded.begin(), expanded.end());
        }
    }
}

RegexNode *Parser::parsePattern() {
    return parseAlt();
}

RegexNode *Parser::parseAlt() {
    RegexNode *left = parseConcat();
    while (peek().type == TOK_PIPE) {
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
        || type == TOK_DEFINITION_REFERENCE) {
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
        case (TOK_LITERAL):
            return new LiteralNode(token.value[0]);
        case (TOK_STRING): {
            RegexNode *result = new LiteralNode(token.value[0]);
            for (size_t i = 1; i < token.value.length(); i++)
                result = new ConcatNode(result, new LiteralNode(token.value[i]));
            return result;
        }
        case (TOK_CHARCLASS): {
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

std::vector<Rule> Parser::parse() {
    parseHeader();
    expandDefinitions();
    parse
}
