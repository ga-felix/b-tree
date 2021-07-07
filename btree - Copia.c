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

int insertOrdered(Record* array, int* size, Record key) {
    int index;
    for(index = 0; index < size && key > array[index]; index++); // Finds insertion index
    array[index] = key;
    *size += 1;
    return index;
}

void copyNode(Node* copy, Node* copied, int from, int to) {
    int index;
    for(index = from; index < to; index++) {
        int aux;
        for(aux = 0; aux < copied->keysNumber - 1; aux++) {
            rightNode->keys[aux] = rightNode->keys[aux + 1];
            free(rightNode->pointers[aux]);
            rightNode->pointers[aux] = rightNode->pointers[aux + 1];
            rightNode->keysNumber -= 1;
        }
    }
}

void splitNode(Node* rightNode, Record key, int whereInsert) {
    Node* parent = rightNode->parent;
    if(!parent) {
        parent = initializeNode();
        parent->isLeaf = false;
    }

    if(parent->keysNumber == MAXKEYS) {
        // DIVIDE PARENT NODE (RECURSIVE SOLUTION?)
    }

    /*
    Se nó pai não existe -> inicializar um
    Se nó pai está cheio -> dividir nó pai
    Escolher o ascendente e torna-lo pai
    Criar nó a esquerda*/
}

void splitNode2(Node* rightNode, Record key, int whereInsert) {
    Node* parent = rightNode->parent;
    if(!parent) {
        parent = initializeNode();
        parent->isLeaf = false;
    }

    if(parent->keysNumber == MAXKEYS) {
        // DIVIDE PARENT NODE (RECURSIVE SOLUTION?)
    }

    int insertion;
    for(insertion = 0; insertion < parent->keysNumber && rightNode->keys[MAXKEYS / 2] > parent->keys[insertion]; insertion++);
    parent->keys[insertion] = rightNode->keys[MAXKEYS / 2]; // Median value sent to parent node
    parent->pointers[insertion + 1] = (void*) rightNode;

    // CREATES NEW CHILD NODE (PRIOR TO LEFT)
    Node* leftNode = initializeNode();
    parent->pointers[insertion] = (void*) leftNode;
    leftNode->parent = parent;
    int index;
    for(index = 0; index < MAXKEYS / 2; index++) {
        leftNode->keys[index] = rightNode->keys[index];
        leftNode->pointers[index] = rightNode->pointers[index];
        leftNode->keysNumber += 1;
    }
    leftNode->pointers[index] = (void*) rightNode;

    // ADJUST ALREADY EXISTENT CHILD NODE (PRIOR TO RIGHT)
    int removal;
    for(removal = 0; removal < MAXKEYS / 2; removal++) {
        int aux;
        for(aux = 0; aux < rightNode->keysNumber - 1; aux++) {
            rightNode->keys[aux] = rightNode->keys[aux + 1];
            free(rightNode->pointers[aux]);
            rightNode->pointers[aux] = rightNode->pointers[aux + 1];
            rightNode->keysNumber -= 1;
        }
        rightNode->keys[keysNumber - 1 - removal] = 0;
        free(rightNode->pointers[keysNumber - 1 - removal]);
        rightNode->pointers[keysNumber - 1 - removal] = rightNode->pointers[keysNumber - removal];
        rightNode->pointers[keysNumber - removal] = NULL;
    }

}

/* Insert a record */

bool insertRecord(Node* tree, Record key) {

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
        insertOrdered(tree->keys, &tree->keysNumber, key);
        return true;
    }


    return true;
    
}

/* Remove a record, if exists */

bool removeRecord(Node* tree, Record key) {
    return true;
}