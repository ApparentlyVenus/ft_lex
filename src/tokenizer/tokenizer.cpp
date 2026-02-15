#include "../../inc/tokenizer/Tokenizer.hpp"

Token::Token(TokenType t, const std::string& v, int line, int col)
    :   type(t), value(v), line(line), column(col) {}

Tokenizer::Tokenizer(const std::string& input) : input(input) {}

char Tokenizer::peek() {
    if (isAtEnd()) return '\0';
    return input[pos];
}

char Tokenizer::peekNext() {
    if (pos + 1 >= input.size()) return '\0';
    return input[pos + 1];
}

char Tokenizer::advance() {
    char c = input[pos];
    pos++;
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

bool Tokenizer::isAtEnd() {
    return pos >= input.size();
}

Token Tokenizer::lexString() {
    int startLine = line;
    int startCol = column;
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
    int startLine = line;
    int startCol = column;
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
    int startLine = line;
    int startCol = column;
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
    int startLine = line;
    int startCol = column;
    
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
        return Token(TOK_PERCENT_RBRACE, "&}", startLine, startCol);
    }
    throw std::runtime_error("unknown directive at line " + std::to_string(startLine));
}

Token Tokenizer::lexRepeatCount() {
    int startLine = line;
    int startCol = column;
    std::string value;

    advance();
    while (!isAtEnd() && peek() != '}') {
        char c = peek();
        if (!std::isdigit(c) && c != ',')
            throw std::runtime_error("invalid repeat at line " + std::to_string(startLine));
        value += advance();
    }
    if (isAtEnd())
        throw std::runtime_error("unterminated repeat count at " + std::to_string(startLine));
    advance();
    return Token(TOK_REPEAT, value, startLine, startCol);
}

Token Tokenizer::lexDefiniton() {
    int startLine = line;
    int startCol = column;
    std::string value;

    while(!isAtEnd() && !std::isspace(peek()))
        value += advance();
    skipWhitespace();
    value += ' ';
    while (!isAtEnd() && peek() != '\n');
        value += advance();
    return Token(TOK_DEFINITION, value, startLine, startCol);  
}

void Tokenizer::skipNewlines() {
    while (!isAtEnd() && std::isspace(peek())) {
        if (peek() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        pos++;
    }
}

void Tokenizer::skipWhitespace() {
    while (!isAtEnd() && (peek() == ' ' || peek() == '\t')) {
        column++;
        pos++;
    }
}

void Tokenizer::tokenizeAction() {
    skipWhitespace();

    if (isAtEnd() || peek() == '\n') {
        advance();
        tokens.push_back(Token(TOK_NEWLINE, "\n", line, column));
        return;
    }
    tokens.push_back(lexCCode());
    if (peek() == '\n') {
        advance();
        tokens.push_back(Token(TOK_NEWLINE, "\n", line, column));
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
            case '{': tokens.push_back(lexRepeatCount()); break;
            case '|': advance(); tokens.push_back(Token(TOK_PIPE,     "|", line, column)); break;
            case '*': advance(); tokens.push_back(Token(TOK_STAR,     "*", line, column)); break;
            case '+': advance(); tokens.push_back(Token(TOK_PLUS,     "+", line, column)); break;
            case '?': advance(); tokens.push_back(Token(TOK_QUESTION, "?", line, column)); break;
            case '(': advance(); tokens.push_back(Token(TOK_LPAREN,   "(", line, column)); break;
            case ')': advance(); tokens.push_back(Token(TOK_RPAREN,   ")", line, column)); break;
            case '.': advance(); tokens.push_back(Token(TOK_DOT,      ".", line, column)); break;
            case '^': advance(); tokens.push_back(Token(TOK_CARET,    "^", line, column)); break;
            case '$': advance(); tokens.push_back(Token(TOK_DOLLAR,   "$", line, column)); break;
            case '\\':
                advance();
                tokens.push_back(Token(TOK_LITERAL, std::string(1, advance()), line, column));
                break;
            default:
                advance();
                tokens.push_back(Token(TOK_LITERAL, std::string(1, c), line, column));
                break;

        }
    } 
}

void Tokenizer::tokenizeRules() {
    while (!isAtEnd) {
        skipNewlines();
        if (peek() == '%' && peekNext() == '%') {
            advance();
            advance();
            tokens.push_back(Token(TOK_PERCENT_PERCENT, "%%", line, column));
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
            tokens.push_back(Token(TOK_PERCENT_PERCENT, "%%", line, column));
            return ;
        }
        if (peek() == '%' && peekNext() == '{') {
            advance();
            advance();
            tokens.push_back(Token(TOK_PERCENT_LBRACE, "%{", line, column));
            
            std::string code;
            while (!isAtEnd()) {
                if (peek() == '%' && peekNext() == '}') {
                    advance();
                    advance();
                    break;
                }
                code += advance();
            }
            tokens.push_back(Token(TOK_C_CODE, code, line, column));
            tokens.push_back(Token(TOK_PERCENT_RBRACE, "%}", line, column));
            continue; 
        }
        tokens.push_back(lexDefiniton());
    }
    throw std::runtime_error("unexcpected EOF: missing %%");
}

void Tokenizer::tokenizeCode() {
    int startline = line;
    int startColumn = column;
    std::string value;

    while (!isAtEnd())
        value += advance();
    if (!value.empty())
        tokens.push_back(Token(TOK_C_CODE, value, startline, column));
}

std::vector<Token> Tokenizer::tokenize() {
    tokenizeHeader();
    tokenizeRules();
    tokenizeCode();
    tokens.push_back(Token(TOK_EOF, "", line, column));
    return tokens;
}