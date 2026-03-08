#ifndef QUESTION_NODE_HPP
# define QUESTION_NODE_HPP

# include "RegexNode.hpp"

class QuestionNode : public UnaryNode {
    public:
    QuestionNode(RegexNode* c);
};

#endif