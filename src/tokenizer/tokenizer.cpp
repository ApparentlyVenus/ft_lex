#include "../../inc/tokenizer/tokenizer.hpp"

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
    return Token(TOK_STRING, val, startLine, startCol)
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