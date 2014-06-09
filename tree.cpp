#include "tree.h"

#include <iostream>
/**
 * @brief Tree::Tree
 *
 * Konstruktor tworzy obiekt drzewa, zawierajcy zadane slowa
 *
 * @param slowa Wektor obiektow klasy std::string zawierający slowa do monitorowania
 */
Tree::Tree(std::vector<std::string> slowa){

    this->root = new Node(NULL);
    this->root->setParent(NULL);
    //this->root->insertCharacter("c")
    for(std::vector<std::string>::iterator it= slowa.begin(); it != slowa.end(); ++it){

        std::string current = *it;
        const char* chars = current.c_str();
        Node* currentNode = this->root;
        for(unsigned int i = 0; i < current.length(); ++i){
            currentNode = currentNode->insertCharacter(chars[i]);
        }
        currentNode->setTerminal(true);

    }
}

Node *Tree::getRoot()
{
    return this->root;
}
