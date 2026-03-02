#include "../inc/tokenizer/Tokenizer.hpp"
#include "../inc/parser/Parser.hpp"
#include "../inc/utils/FileUtils.hpp"
#include "../inc/utils/Logger.hpp"
#include <iostream>

void printTokens(const std::vector<Token>& tokens) {
    std::cout << "\n=== TOKENS ===\n";
    for (const auto& tok : tokens) {
        std::cout << tok << "\n";
    }
}

void printAST(RegexNode* node, int depth = 0) {
    std::string indent(depth * 2, ' ');
    
    if (LiteralNode* lit = dynamic_cast<LiteralNode*>(node)) {
        std::cout << indent << "Literal('" << lit->getChar() << "')\n";
    } else if (CharClassNode* cc = dynamic_cast<CharClassNode*>(node)) {
        std::cout << indent << "CharClass(" << cc->getPattern() 
                  << (cc->isNegated() ? ", negated" : "") << ")\n";
    } else if (DotNode* dot = dynamic_cast<DotNode*>(node)) {
        std::cout << indent << "Dot\n";
        (void)dot;
    } else if (StarNode* star = dynamic_cast<StarNode*>(node)) {
        std::cout << indent << "Star\n";
        printAST(star->getChild(), depth + 1);
    } else if (PlusNode* plus = dynamic_cast<PlusNode*>(node)) {
        std::cout << indent << "Plus\n";
        printAST(plus->getChild(), depth + 1);
    } else if (QuestionNode* q = dynamic_cast<QuestionNode*>(node)) {
        std::cout << indent << "Question\n";
        printAST(q->getChild(), depth + 1);
    } else if (ConcatNode* concat = dynamic_cast<ConcatNode*>(node)) {
        std::cout << indent << "Concat\n";
        printAST(concat->getLeft(), depth + 1);
        printAST(concat->getRight(), depth + 1);
    } else if (AltNode* alt = dynamic_cast<AltNode*>(node)) {
        std::cout << indent << "Alt\n";
        printAST(alt->getLeft(), depth + 1);
        printAST(alt->getRight(), depth + 1);
    }
}

void printRules(const std::vector<Rule>& rules) {
    std::cout << "\n=== RULES ===\n";
    for (size_t i = 0; i < rules.size(); i++) {
        std::cout << "Rule " << i << " (priority " << rules[i].priority << "):\n";
        std::cout << "  Pattern AST:\n";
        printAST(rules[i].pattern, 2);
        std::cout << "  Action: \"" << rules[i].action << "\"\n\n";
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        logError("Usage: ./ft_lex <file.l>");
        return 1;
    }

    try {
        // Read input file
        std::string input = readFile(argv[1]);
        if (input.empty()) {
            logError("Failed to read file: " + std::string(argv[1]));
            return 1;
        }

        // Tokenize
        logInfo("Tokenizing...");
        Tokenizer tokenizer(input);
        std::vector<Token> tokens = tokenizer.tokenize();
        printTokens(tokens);

        // Parse
        logInfo("Parsing...");
        Parser parser(tokens);
        std::vector<Rule> rules = parser.parse();
        printRules(rules);

        logInfo("Success!");

    } catch (const std::exception& e) {
        logError(std::string("Error: ") + e.what());
        return 1;
    }

    return 0;
}