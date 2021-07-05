#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

/* Returns initialized B+ Tree's root node */

Node* initializeTree() {
    Node* root = NULL;

    if(!(root = (Node*) calloc(1, sizeof(Node)))) {
        fprintf(stderr, "[B+TREE] Initialization failed");
        exit(1);
    }
    
    root->parent = NULL;
    root->pointers = NULL;
    root->keys = NULL;
    root->isLeaf = true;
    root->keysNumber = 0;
    return root;
}

/* Print the tree. Check "enunciado.pdf" to
further instructions (there are many!) */

bool printTree(Node* tree) {
    return true;
}

/* Insert a record */

bool insertRecord(Node* tree, Record key) {
    return true;
}

/* Remove a record, if exists */

bool removeRecord(Node* tree, Record key) {
    return true;
}