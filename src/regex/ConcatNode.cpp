#include "../../inc/regex/ConcatNode.hpp"

ConcatNode::ConcatNode(RegexNode *l, RegexNode *r) : BinaryNode(l, r) {}
