#ifndef KEYMAP_HPP_
#define KEYMAP_HPP_

#include "Key.hpp"
#include <iostream>
#include <stdexcept>

class KeyMap {

struct Node {
    Node* previous = nullptr;
    Node* next = nullptr;
    bool isActive = false;
    Key key;
};

private:
    Node nodeArray[128];
    Node* linkedListStart;
    Node* linkedListEnd;
    Node* linkedListCurrent;

public:
    KeyMap () :
        linkedListStart (nullptr),
        linkedListEnd (nullptr),
        linkedListCurrent (nullptr)
    {
    }

    inline Key* getKey(int index) {
        Node* n = &nodeArray[index];

        if (!n->isActive) {
            if (!linkedListStart) {
                linkedListStart = n;
                n->previous = nullptr;
            } else {
                linkedListEnd->next = n;
                n->previous = linkedListEnd;
            }

            n->next = nullptr;
            linkedListEnd = n;
            n->isActive = true;
        }
        return &(n->key);
    }

    inline void erasePrevious() {
        if (linkedListCurrent) {
            eraseNode(linkedListCurrent->previous);
        } else {
            // We proceeded past the end
            if (linkedListEnd) eraseNode(linkedListEnd);
            else throw std::invalid_argument ("shouldn't call erasePrev on empty list");
        }
    }

    inline void eraseNode(Node* n) {
        if (!n) throw std::invalid_argument ("eraseNode called on nullptr node");
        if (!n->isActive) throw std::invalid_argument ("trying to erase inactive node");

        if (n->next) n->next->previous = n->previous;
        else linkedListEnd = n->previous;

        if (n->previous) n->previous->next = n->next;
        else linkedListStart = n->next;

        n->isActive = false;
    }

    inline void startLoop() {
        linkedListCurrent = linkedListStart;
    }

    inline Key* getNext() {
        if (!linkedListCurrent || !linkedListCurrent->isActive) return nullptr;
        Key* returnKey = &linkedListCurrent->key;

        linkedListCurrent = linkedListCurrent->next;
        return returnKey;
    }
};



#endif