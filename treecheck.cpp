#include "treecheck.h"

/**
 * @brief TreeCheck::TreeCheck
 * Inicjalizacja obiektu odpowiedzialnego za sprawdzanie czy zadane znaki znajduja sie w drzewie
 * @param tree drzewo slow
 */
TreeCheck::TreeCheck(Tree* tree)
{
    this->tree = tree;
    this->currentNode = tree->getRoot();
}

/**
 * @brief TreeCheck::checkNextLetter
 *
 * Sprawdzenie czy w kolejna litera z wprowadzonego ciagu jest zgodna z jedna z kolejnych liter w drzewie
 * @param c
 * @return
 */
bool TreeCheck::checkNextLetter(char c)
{
    Node *tmp = this->currentNode->getNodeForCharacter(c);

    if(tmp != NULL)
    {
        this->currentNode = tmp;
        return true;
    } else
    {
        this->currentNode = tree->getRoot();
        return false;
    }
}

/**
 * @brief TreeCheck::isCurrentNodeTerminal
 * @return zwraca true jesli obecna pozycja w drzewie odpowiada jednemu z wezlow terminalnych(od korzenia do tego wezla znajduje sie slowo)
 */
bool TreeCheck::isCurrentNodeTerminal()
{
    return this->currentNode->getTerminal();

}

/**
 * @brief TreeCheck::getFoundWord
 * @return Slowo od korzenia do obecnego wezla, jesli terminalny, pusty ciag w przeciwnym wypadku
 */
std::string TreeCheck::getFoundWord()
{
    if(this->currentNode->getTerminal() == true)
        return this->currentNode->getWord();
    else
        return "";
}
