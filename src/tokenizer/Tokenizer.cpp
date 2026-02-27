#include "../../inc/tokenizer/Tokenizer.hpp"

Token::Token(TokenType t, const std::string& v, int line, int col)
    :   type(t), value(v), line(line), column(col) {}

Tokenizer::Tokenizer(const std::string& input) 
    : _input(input), _pos(0), _line(1), _column(1), tokens() {}

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

    while(!isAtEnd()) {
        if (peek() == ']') {
            if (val.length() > 0 && val.back() == ':') {
                val += advance();
                continue;
            }
            break;
        }
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
        val += advance();
        braceDepth = 1;
        while (!isAtEnd() && braceDepth > 0) {
            char c = advance();
            val += c;
            if (c == '{')
                braceDepth++;
            else if (c == '}')
                braceDepth--;
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

    if (value.empty())
        throw std::runtime_error("empty brace at line " + std::to_string(_line));

    bool isRepeat = true;
    for (size_t i = 0; i < value.length(); i++) {
        if (!std::isdigit(value[i]) && value[i] != ',') {
            isRepeat = false;
            break;
        }
    }
    if (isRepeat)
        return Token(TOK_REPEAT, value, startLine, startCol);
    return Token(TOK_DEFINITION_REFERENCE, value, startLine, startCol);
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

void Tokenizer::skipComment() {
    if (peek() == '/' && peekNext() == '*') {
        advance(); advance();
        while (!isAtEnd()) {
            if (peek() == '*' && peekNext() == '/') {
                advance(); advance();
                return;
            }
            advance();
        }
        throw std::runtime_error("unterminated comment at line " + std::to_string(_line));
    }
}

void Tokenizer::tokenizeAction() {
    skipWhitespace();

    if (isAtEnd() || peek() == '\n') {
        tokens.push_back(Token(TOK_C_CODE, "", _line, _column));
        if (peek() == '\n')
            advance();
        tokens.push_back(Token(TOK_NEWLINE, "\n", _line, _column));
        return;
    }

    if (peek() == '|') {
        advance();
        tokens.push_back(Token(TOK_PIPE, "|", _line, _column));
        if (peek() == '\n') {
            advance();
            tokens.push_back(Token(TOK_NEWLINE, "\n", _line, _column));
        }
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
        skipNewlines();

        if (peek() == '/' && peekNext() == '*') {
            skipComment();
            continue;
        }
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
        if (peek() == '\n')
            advance();
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
        tokens.push_back(Token(TOK_C_CODE, value, startline, startColumn));
}

std::vector<Token> Tokenizer::tokenize() {
    tokenizeHeader();
    tokenizeRules();
    tokenizeCode();
    tokens.push_back(Token(TOK_EOF, "", _line, _column));
    return tokens;
}

const char* tokenTypeToString(TokenType type) {
    switch(type) {
        case TOK_PERCENT_PERCENT: return "TOK_PERCENT_PERCENT";
        case TOK_PERCENT_LBRACE: return "TOK_PERCENT_LBRACE";
        case TOK_PERCENT_RBRACE: return "TOK_PERCENT_RBRACE";
        case TOK_PIPE: return "TOK_PIPE";
        case TOK_STAR: return "TOK_STAR";
        case TOK_PLUS: return "TOK_PLUS";
        case TOK_QUESTION: return "TOK_QUESTION";
        case TOK_LPAREN: return "TOK_LPAREN";
        case TOK_RPAREN: return "TOK_RPAREN";
        case TOK_LBRACKET: return "TOK_LBRACKET";
        case TOK_RBRACKET: return "TOK_RBRACKET";
        case TOK_LBRACE: return "TOK_LBRACE";
        case TOK_RBRACE: return "TOK_RBRACE";
        case TOK_CARET: return "TOK_CARET";
        case TOK_DOLLAR: return "TOK_DOLLAR";
        case TOK_DOT: return "TOK_DOT";
        case TOK_LITERAL: return "TOK_LITERAL";
        case TOK_STRING: return "TOK_STRING";
        case TOK_CHARCLASS: return "TOK_CHARCLASS";
        case TOK_NUMBER: return "TOK_NUMBER";
        case TOK_C_CODE: return "TOK_C_CODE";
        case TOK_REPEAT: return "TOK_REPEAT";
        case TOK_DEFINITION: return "TOK_DEFINITION";
        case TOK_DEFINITION_REFERENCE: return "TOK_DEFINITION_REFERENCE";
        case TOK_NEWLINE: return "TOK_NEWLINE";
        case TOK_WHITESPACE: return "TOK_WHITESPACE";
        case TOK_EOF: return "TOK_EOF";
        case TOK_ERROR: return "TOK_ERROR";
        default: return "UNKNOWN";
    }
}


std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token " << tokenTypeToString(token.type) << " at " << token.line << ":" << token.column << " val = " << token.value;
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Token>& tokens) {
    for (size_t i = 0; i < tokens.size(); i++) {
        os << tokens[i];
        if (i < tokens.size() - 1) os << "\n";
    }
    return os;
}