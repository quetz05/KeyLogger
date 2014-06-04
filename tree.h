#ifndef TREE_H
#define TREE_H

#include "node.h"
#include <string>
#include <vector>

class Tree{
public:
    Tree(std::vector<std::string> slowa);
    Node *getRoot();

private:
    Node* root;

};

#endif // TREE_H
