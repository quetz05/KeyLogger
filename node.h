#ifndef NODE_H
#define NODE_H

#include <map>
#include <string>

class Node {

public:
    Node(Node *parent, char c);
    Node(Node *parent);
    Node *insertCharacter(char c);
    char getChar();
    Node *getNodeForCharacter(char c);
    int childCount();
    std::string getWord();
    void setParent(Node* n);


    bool operator==(const Node& node){

       return ((this->parent == node.parent) && (this->znak == node.znak));

    }

    bool getTerminal() const;
    void setTerminal(bool value);

private:
    Node* parent;
    char znak;
    std::map<char, Node*> nodes;
    bool terminal;
};

#endif // NODE_H
