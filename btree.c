#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

/* Returns initialized B+ Tree's node */

Node* initializeNode() {
    Node* root = NULL;

    if(!(root = (Node*) calloc(1, sizeof(Node)))) {
        fprintf(stderr, "[B+TREE] Initialization failed");
        exit(1);
    }
    
    root->parent = NULL;
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

    int index;
    for(index = 0; index < tree->keysNumber && key > tree->keys[index]; index++); // Finds insertion index

    /* TODO: SPLITTING PROCESS
    You must consider two cases:
    1 - The parent node exists and isn't full, so splitting a full child node is possible
    without creating a new parent node.
    2 - Parent node doesn't exists or it's full. Then, what should happen?
    For now, insertion is possible if there isn't any node full.
    */

    if(tree->keysNumber == MAXKEYS) { // Begins the process of splitting the full node
        int promoted = (MAXKEYS) / 2; // The median node, who should become the parent node after splitting
        Node* newParent = initializeNode();
    }

    if(tree->isLeaf) { // If it's leaf and not full, then insert
        tree->keys[index] = key;
        tree->keysNumber += 1;
        return true;
    }

    if(tree->pointers[index]) { // If it's not leaf and not full, then downin the search
        insertRecord((Node*) tree->pointers[index], key);
    }

    return true;
    
}

/* Remove a record, if exists */

bool removeRecord(Node* tree, Record key) {
    return true;
}