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
    TOK_DOT,

    TOK_LITERAL,
    TOK_STRING,
    TOK_CHARCLASS,
    TOK_NUMBER,
    TOK_C_CODE,
    TOK_REPEAT,
    TOK_DEFINITION,

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
    std::string _input;
    size_t _pos;
    int _line;
    int _column;

    std::vector<Token> tokens;

    char peek();
    char peekNext();
    char advance();
    bool isAtEnd();

    void skipNewlines();
    void skipWhitespace();

    Token lexString();
    Token lexCharClass();
    Token lexCCode();
    Token lexPercentDirective();
    Token lexBrace();
    Token lexDefiniton();

    void tokenizeAction();
    void tokenizePattern();
    void tokenizeRules();
    void tokenizeHeader();
    void tokenizeCode();

    public:
    Tokenizer(const std::string& input);
    std::vector<Token> tokenize();
};


#endif