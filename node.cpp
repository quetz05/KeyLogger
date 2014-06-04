#include "node.h"
#include <exception>
struct UnexpectedException : public std::exception
{
   std::string s;
   UnexpectedException(std::string ss) : s(ss) {}
   ~UnexpectedException() throw () {} // Updated
   const char* what() const throw() { return s.c_str(); }
};

Node::Node(Node *parent, char c)
{
    this->parent = parent;
    this->znak = c;
    this->terminal = false;
}

Node::Node(Node *parent)
{
    Node(parent, -1);
}

Node *Node::insertCharacter(char c)
{
    if(this->nodes.count(c) == 0){
        Node *tmp = new Node(this, c);

        this->nodes.insert(std::pair<char, Node*>(c,tmp));
        return tmp;
    }else{
        std::map<char, Node*>::iterator it = this->nodes.find(c);
        return (*it).second;
    }
}

char Node::getChar()
{
    return this->znak;
}

Node *Node::getNodeForCharacter(char c)
{
    if(this->nodes.count(c) == 0){

        return NULL;
    }else{
        std::map<char, Node*>::iterator it = this->nodes.find(c);
        return (*it).second;
    }
}

int Node::childCount()
{
    return this->nodes.size();
}

std::string Node::getWord()
{

    Node* node = this;
    std::string word;

    while(node->parent != NULL){
        word.insert(word.begin(), node->getChar());
        node = node->parent;
    }

    return word;
}


bool Node::getTerminal() const
{
    return terminal;
}

void Node::setTerminal(bool value)
{
    terminal = value;
}

