#ifndef TOKENIZER_HPP
# define TOKENIZER_HPP

# include <vector>
# include <string>
# include <stdexcept>

typedef enum {
    TOK_PERCENT_PERCENT,
    TOK_PERCENT_LBRACE,
    TOK_PERCENT_RBRACE,

    TOK_PIPE,
    TOK_STAR,
    TOK_PLUS,
    TOK_QUESTION,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_CARET,
    TOK_DOLLAR,

    TOK_LITERAL,
    TOK_STRING,
    TOK_CHARCLASS,
    TOK_NUMBER,
    TOK_C_CODE,

    TOK_NEWLINE,
    TOK_WHITESPACE,
    TOK_EOF,
    TOK_ERROR,
} TokenType;

typedef struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType type, const std::string& value, int line, int col);

} Token;

class Tokenizer {
    private:
    std::string input;
    size_t pos;
    int line;
    int column;

    std::vector<Token> tokens;

    char peek();
    char peekNext();
    char advance();
    bool isAtEnd();

    Token   makeToken();
    Token   lexString();
    Token   lexCharClass();
    Token   lexCCode();
    Token   lexPercentDirective();

    public:
    Tokenizer(const std::string& input);
    std::vector<Token> tokenize();
};


#endif