#include "../../inc/tokenizer/Tokenizer.hpp"

Token::Token(TokenType t, const std::string& v, int line, int col)
    :   type(t), value(v), line(line), column(col) {}

Tokenizer::Tokenizer(const std::string& input) 
    : _input(input), _pos(0), _line(1), _column(1) {}

char Tokenizer::peek() {
    if (isAtEnd()) return '\0';
    return _input[_pos];
}

char Tokenizer::peekNext() {
    if (_pos + 1 >= _input.size()) return '\0';
    return _input[_pos + 1];
}

char Tokenizer::advance() {
    char c = _input[_pos];
    _pos++;
    if (c == '\n') {
        _line++;
        _column = 1;
    } else {
        _column++;
    }
    return c;
}

bool Tokenizer::isAtEnd() {
    return _pos >= _input.size();
}

Token Tokenizer::lexString() {
    int startLine = _line;
    int startCol = _column;
    std::string val;

    advance();
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\') {
            val += advance();
            val += advance();
        } else {
            val += advance();
        }
    } 
    if (isAtEnd())
        throw std::runtime_error("unterminated string at line " + std::to_string(startLine));
    advance();
    return Token(TOK_STRING, val, startLine, startCol);
}

Token Tokenizer::lexCharClass() {
    int startLine = _line;
    int startCol = _column;
    std::string val;

    advance();
    if (peek() == '^')
        val += advance();
    if (peek() == ']')
        val += advance();
    while(!isAtEnd() && peek() != ']') {
        if (peek() == '\\') {
            val += advance();
            val += advance();
        } else {
            val += advance();
        }
    }
    if (isAtEnd())
        throw std::runtime_error("unterminated character class at line " + std::to_string(startLine));
    
    advance();
    return Token(TOK_CHARCLASS, val, startLine, startCol);
}

Token Tokenizer::lexCCode() {
    int startLine = _line;
    int startCol = _column;
    std::string val;

    int braceDepth = 0;

    if (peek() == '{') {
        advance();
        braceDepth++;
        while (!isAtEnd() && braceDepth > 0) {
            char c = advance();
            if (c == '{')
                braceDepth++;
            else if (c == '}')
                braceDepth--;
            else
                val += c;
        }
    } else {
        while (!isAtEnd() && peek() != '\n')
            val += advance();
    }
    return Token(TOK_C_CODE, val, startLine, startCol);
}

Token Tokenizer::lexPercentDirective() {
    int startLine = _line;
    int startCol = _column;
    
    advance();
    if (peek() == '%') {
        advance();
        return Token(TOK_PERCENT_PERCENT, "%%", startLine, startCol);
    }
    if (peek() == '{') {
        advance();
        return Token(TOK_PERCENT_LBRACE, "%{", startLine, startCol);
    }
    if (peek() == '}') {
        advance();
        return Token(TOK_PERCENT_RBRACE, "%}", startLine, startCol);
    }
    throw std::runtime_error("unknown directive at line " + std::to_string(startLine));
}

Token Tokenizer::lexBrace() {
    int startLine = _line;
    int startCol = _column;
    std::string value;

    advance();
    while (!isAtEnd() && peek() != '}') {
        value += advance();
    }
    advance();

    bool isRepeat = true;
    for (int i = 0; i < value.length(); i++) {
        if (!std::isdigit(value[0]) && value[0] != ',') {
            isRepeat = false;
            break;
        }
    }
    if (isRepeat)
        return Token(TOK_REPEAT, value, startLine, startCol);
    return Token(TOK_DEFINITION, value, startLine, startCol);
}

Token Tokenizer::lexDefiniton() {
    int startLine = _line;
    int startCol = _column;
    std::string value;

    while(!isAtEnd() && !std::isspace(peek()))
        value += advance();
    skipWhitespace();
    value += ' ';
    while (!isAtEnd() && peek() != '\n')
        value += advance();
    return Token(TOK_DEFINITION, value, startLine, startCol);  
}

void Tokenizer::skipNewlines() {
    while (!isAtEnd() && std::isspace(peek())) {
        if (peek() == '\n') {
            _line++;
            _column = 1;
        } else {
            _column++;
        }
        _pos++;
    }
}

void Tokenizer::skipWhitespace() {
    while (!isAtEnd() && (peek() == ' ' || peek() == '\t')) {
        _column++;
        _pos++;
    }
}

void Tokenizer::tokenizeAction() {
    skipWhitespace();

    if (isAtEnd() || peek() == '\n') {
        advance();
        tokens.push_back(Token(TOK_NEWLINE, "\n", _line, _column));
        return;
    }
    tokens.push_back(lexCCode());
    if (peek() == '\n') {
        advance();
        tokens.push_back(Token(TOK_NEWLINE, "\n", _line, _column));
    }
}

void Tokenizer::tokenizePattern() {
    while (!isAtEnd() && peek() != '\n') {
        if (std::isspace(peek()) && peek() != '\n')
            return ;
        char c = peek();
        switch(c) {
            case '"': tokens.push_back(lexString()); break;
            case '[': tokens.push_back(lexCharClass()); break;
            case '{': tokens.push_back(lexBrace()); break;
            case '|': advance(); tokens.push_back(Token(TOK_PIPE,     "|", _line, _column)); break;
            case '*': advance(); tokens.push_back(Token(TOK_STAR,     "*", _line, _column)); break;
            case '+': advance(); tokens.push_back(Token(TOK_PLUS,     "+", _line, _column)); break;
            case '?': advance(); tokens.push_back(Token(TOK_QUESTION, "?", _line, _column)); break;
            case '(': advance(); tokens.push_back(Token(TOK_LPAREN,   "(", _line, _column)); break;
            case ')': advance(); tokens.push_back(Token(TOK_RPAREN,   ")", _line, _column)); break;
            case '.': advance(); tokens.push_back(Token(TOK_DOT,      ".", _line, _column)); break;
            case '^': advance(); tokens.push_back(Token(TOK_CARET,    "^", _line, _column)); break;
            case '$': advance(); tokens.push_back(Token(TOK_DOLLAR,   "$", _line, _column)); break;
            case '\\':
                advance();
                tokens.push_back(Token(TOK_LITERAL, std::string(1, advance()), _line, _column));
                break;
            default:
                advance();
                tokens.push_back(Token(TOK_LITERAL, std::string(1, c), _line, _column));
                break;

        }
    } 
}

void Tokenizer::tokenizeRules() {
    while (!isAtEnd()) {
        skipNewlines();
        if (peek() == '%' && peekNext() == '%') {
            advance();
            advance();
            tokens.push_back(Token(TOK_PERCENT_PERCENT, "%%", _line, _column));
            return ;
        }
        tokenizePattern();
        tokenizeAction();
    }
}

void Tokenizer::tokenizeHeader() {
    while (!isAtEnd()) {
        if (peek() == '%' && peekNext() == '%') {
            advance();
            advance();
            tokens.push_back(Token(TOK_PERCENT_PERCENT, "%%", _line, _column));
            return ;
        }
        if (peek() == '%' && peekNext() == '{') {
            advance();
            advance();
            tokens.push_back(Token(TOK_PERCENT_LBRACE, "%{", _line, _column));
            
            std::string code;
            while (!isAtEnd()) {
                if (peek() == '%' && peekNext() == '}') {
                    advance();
                    advance();
                    break;
                }
                code += advance();
            }
            tokens.push_back(Token(TOK_C_CODE, code, _line, _column));
            tokens.push_back(Token(TOK_PERCENT_RBRACE, "%}", _line, _column));
            continue; 
        }
        tokens.push_back(lexDefiniton());
    }
    throw std::runtime_error("unexcpected EOF: missing %%");
}

void Tokenizer::tokenizeCode() {
    int startline = _line;
    int startColumn = _column;
    std::string value;

    while (!isAtEnd())
        value += advance();
    if (!value.empty())
        tokens.push_back(Token(TOK_C_CODE, value, startline, _column));
}

std::vector<Token> Tokenizer::tokenize() {
    tokenizeHeader();
    tokenizeRules();
    tokenizeCode();
    tokens.push_back(Token(TOK_EOF, "", _line, _column));
    return tokens;
}