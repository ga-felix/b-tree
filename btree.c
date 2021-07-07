#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

/* Print the tree. Check "enunciado.pdf" to
further instructions (there are many!) */

bool printTree(Node* tree) {
    return true;
}


/* Remove a record, if exists */

bool removeRecord(Node* tree, Record key) {
    return true;
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

/* Insert a record in the tree 
@params: Node* root -> B+ tree's root, Record key -> record to be inserted
@returns: boolean that indicates if op succeded */

bool insertRecord(Node* root, Record key) {
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
        }
        cursor = (Node*) cursor->pointers[index]; // Key is greater than all keys here
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
        newLeaf->keysNumber = (MAXKEYS + 1) / 2;
        cursor->pointers[cursor->keysNumber] = (void*) newLeaf; // Leaf nodes points to other leaf node
        newLeaf->pointers[newLeaf->keysNumber] = cursor->pointers[MAXKEYS + 1]; // Leaf nodes points to other leaf node
        cursor->pointers[MAXKEYS + 1] = NULL;
        for(index = 0; index < cursor->keysNumber; index++) { // Giving back the keys correctly
            cursor->keys[index] = temporaryNode[index];
        }
        for(index = 0, end = cursor->keysNumber; index < newLeaf->keysNumber; index++, end++) { // Giving back the keys correctly
            newLeaf->keys[index] = temporaryNode[end];
        }
        if(cursor == root) { // If it's root node
            Node* newRoot = createNode(false);
            newRoot->keys[0] = newLeaf->keys[0];
            newRoot->pointers[0] = (void*) cursor;
            newRoot->pointers[1] = (void*) newLeaf;
            newRoot->keysNumber = 1;
        } else { // Hardest of hardest... Splitting time.

        }

    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////