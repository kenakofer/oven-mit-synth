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

    inline void eraseIndex(int index) {
        Node* n = &nodeArray[index];
        if (!n->isActive) return;
        eraseNode(n);
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

    inline Key* getStart() {
        if (!linkedListStart) return nullptr;
        return &linkedListStart->key;
    }

    inline Key* getEnd() {
        if (!linkedListEnd) return nullptr;
        return &linkedListEnd->key;
    }

    inline bool hasAtLeast(int num) {
        Node* n = linkedListStart;
        while (num > 0) {
            if (!n) return false;
            n = n->next;
            num--;
        }
        return true;
    }

    inline int getCount() {
        Node* n = linkedListStart;
        int count = 0;
        while (n) {
            count += 1;
            n = n->next;
        }
        return count;
    }

    inline Key* rotateKeyOrder() {
        if (!linkedListStart) return nullptr;
        if (!linkedListStart->next) return &linkedListStart->key;
        int index = linkedListStart->key.note;
        eraseNode(linkedListStart);
        return getKey(index);
    }
};



#endif