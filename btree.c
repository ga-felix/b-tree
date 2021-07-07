#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

/* Print the tree. Check "enunciado.pdf" to
further instructions (there are many!) */

void printTree(Node* tree) {
}


/* Remove a record, if exists */

void removeRecord(Node* tree, Record key) {
}

///////////////////////////////////////////////////////////////////////////////////////////////

/* Create generic node
@params: bool isLeaf -> if true, then the created node is a leaf.
@returns: a new generic node. */

Node* createNode(bool isLeaf) {
    Node* node = NULL;
    if(!(node = (Node*) calloc(1, sizeof(Node)))) {
        fprintf(stderr, "[B+TREE] Node initialization failed");
        exit(1);
    }

    if(!(node->keys = (Record*) calloc(MAXKEYS, sizeof(Record)))) {
        fprintf(stderr, "[B+TREE] Keys creation failed");
        exit(1);
    }

    if(!(node->pointers = (void**) calloc(MAX, sizeof(void*)))) {
        fprintf(stderr, "[B+TREE] Pointers creation failed");
        exit(1);
    }
    node->parent = NULL;
    node->keysNumber = 0;
    node->isLeaf = isLeaf;
    return node;
}
/*
Node* findParent(Node* cursor, Node* child) {
    if(cursor->isLeaf || cursor->pointers[0]->isLeaf) {
        fprintf(stderr, "[B+TREE] Cursor node it's a leaf");
        return NULL;
    }
    int index;
    for(index = 0; index < cursor->keysNumber + 1; index++) {

    }
}*/

void insertInternal(Node* cursor, Node* child, Record key) {
    if(cursor->keysNumber < MAXKEYS) {
        int index;
        for(index = 0; index < cursor->keysNumber && key > cursor->keys[index]; index++);
        int end;
        for(end = cursor->keysNumber; end > index; end--) {
            cursor->keys[end] = cursor->keys[end - 1];
        }
        for(end = cursor->keysNumber + 1; end > index + 1; end--) {
            cursor->pointers[end] = cursor->pointers[end - 1];
        }
        cursor->keys[index] = key;
        cursor->keysNumber++;
        cursor->pointers[index + 1] = child;
        child->parent = cursor;
    } else {
        Node* newNode = createNode(false);
        int temporaryNode[MAXKEYS + 1];
        void** temporaryPointers;
        if(!(temporaryPointers = (void**) calloc(MAXKEYS + 2, sizeof(void*)))) {
            fprintf(stderr, "[B+TREE] Node initialization failed");
            exit(1);
        }
        int index;
        for(index = 0; index < MAXKEYS; index++) {
            temporaryNode[index] = cursor->keys[index];
        }
        for(index = 0; index < MAXKEYS + 1; index++) {
            temporaryPointers[index] = (void*) cursor->pointers[index];
        }
        int end;
        for(index = 0; index < MAXKEYS && key > temporaryNode[index]; index++);
        for(end = MAXKEYS + 1; end > index; end--) {
            temporaryNode[end] = temporaryNode[end - 1];
        }
        temporaryNode[index] = key;
        for(end = MAXKEYS + 2; end > index + 1; end--) {
            temporaryPointers[end] = temporaryPointers[end - 1];
        }
        temporaryPointers[index + 1] = (void*) child;
        cursor->keysNumber = (MAXKEYS + 1) / 2;
        newNode->keysNumber = MAXKEYS - (MAXKEYS + 1) / 2;
        for(index = 0, end = cursor->keysNumber + 1; index < newNode->keysNumber; index++, end++) {
            newNode->keys[index] = temporaryNode[end];
        }
        for(index = 0, end = cursor->keysNumber + 1; index < newNode->keysNumber + 1; index++, end++) {
            newNode->pointers[index] = (Node*) temporaryPointers[end];
        }

        if(!cursor->parent) {
            Node* newRoot = createNode(false);
            newRoot->keys[0] = cursor->keys[cursor->keysNumber];
            newRoot->pointers[0] = (void*) cursor;
            newRoot->pointers[1] = (void*) newNode;
            newRoot->keysNumber = 1;
            cursor->parent = newRoot;
            newNode->parent = newRoot;
        } else {
            insertInternal(cursor->parent, newNode, cursor->keys[cursor->keysNumber]);
        }
    }

}

/* Insert a record in the tree 
@params: Node* root -> B+ tree's root, Record key -> record to be inserted
@returns: boolean that indicates if op succeded */

void insertRecord(Node* root, Record key) {
    if(!root) { // If given tree is invalid
        fprintf(stderr, "[B+TREE] Cannot insert into NULL tree reference");
        exit(1);
    }

    Node* cursor = root;
    Node* parent = NULL;

    while(!cursor->isLeaf) { // While a leaf isn't reached
        parent = cursor;
        int index;
        for(index = 0; index < cursor->keysNumber; index++) {
            if(key < cursor->keys[index]) {
                cursor = (Node*) cursor->pointers[index];
                break;
            }
            if(index == cursor->keysNumber - 1) {
                cursor = (Node*) cursor->pointers[index + 1]; // Key can be greater than all keys here
                break;
            }
        }
    }

    if(cursor->keysNumber < MAXKEYS) { // If node is not full (easiest case)
        int index;
        for(index = 0; index < cursor->keysNumber && key > cursor->keys[index]; index++);
        int end;
        for(end = cursor->keysNumber; end > index; end--) {
            cursor->keys[end] = cursor->keys[end - 1]; // Shift right one space
        }
        cursor->keys[index] = key; // Inserts the record
        cursor->keysNumber++;

        cursor->pointers[cursor->keysNumber] = cursor->pointers[cursor->keysNumber - 1];
        cursor->pointers[cursor->keysNumber - 1] = NULL;
    } else { // Node is full! (hardest case)
        Node* newLeaf = createNode(true);
        Record temporaryNode[MAXKEYS + 1]; // Extra space will be needed to find 'median' later
        int index;
        for(index = 0; index < MAXKEYS; index++) {
            temporaryNode[index] = cursor->keys[index]; // Copy keys
        }
        for(index = 0; index < MAXKEYS && key > temporaryNode[index]; index++);
        int end;
        for(end = MAXKEYS + 1; end > index; end--) {
            temporaryNode[end] = temporaryNode[end - 1]; // Shift right one space
        }
        temporaryNode[index] = key; // Inserts key on temporary overflowed storage
        cursor->keysNumber = (MAXKEYS + 1) / 2; // Splitting nodes
        newLeaf->keysNumber = MAXKEYS + 1 - (MAXKEYS + 1) / 2;
        cursor->pointers[cursor->keysNumber] = (void*) newLeaf; // Leaf nodes points to other leaf node
        newLeaf->pointers[newLeaf->keysNumber] = cursor->pointers[MAXKEYS]; // Leaf nodes points to other leaf node
        cursor->pointers[MAXKEYS] = NULL;
        for(index = 0; index < cursor->keysNumber; index++) { // Giving back the keys correctly
            cursor->keys[index] = temporaryNode[index];
        }
        for(index = 0, end = cursor->keysNumber; index < newLeaf->keysNumber; index++, end++) { // Giving back the keys correctly
            newLeaf->keys[index] = temporaryNode[end];
        }
        if(!cursor->parent) { // If it's root node
            Node* newRoot = createNode(false);
            newRoot->keys[0] = newLeaf->keys[0];
            newRoot->pointers[0] = (void*) cursor;
            newRoot->pointers[1] = (void*) newLeaf;
            newRoot->keysNumber = 1;
            cursor->parent = newRoot;
            newLeaf->parent = newRoot;
        } else { // Hardest of hardest... Splitting time.
            insertInternal(parent, newLeaf, newLeaf->keys[0]);
        }

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////