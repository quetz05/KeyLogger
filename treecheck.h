#ifndef TREECHECK_H
#define TREECHECK_H

#include "tree.h"
#include "node.h"
class TreeCheck
{
public:
    TreeCheck(Tree* tree);
    bool checkNextLetter(char c);
    bool isCurrentNodeTerminal();
    std::string getFoundWord();
private:
    Tree *tree;
    Node *currentNode;

};

#endif // TREECHECK_H
