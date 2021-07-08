#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

/* Print the tree. Check "enunciado.pdf" to
further instructions (there are many!) 
@params: Node* cursor -> It must be root level node, FILE* output -> File to write output */

void printTree(Node* cursor, FILE* output) {
    fprintf(output, "(");
    if(cursor->isLeaf) {
        int index;
        for(index = 0; index < cursor->keysNumber - 1; index++) {
            fprintf(output, "%d ", cursor->keys[index]);
        }
        fprintf(output, "%d)", cursor->keys[index]);
        return;
    }

    int index;
    int end;
    Node* child;
    for(index = 0, end = 0; index < cursor->keysNumber + 1; index++) {
        child = (Node*) cursor->pointers[index];
        printTree(child, output);
        if(end < cursor->keysNumber) {
            fprintf(output, " %d ", cursor->keys[end]);
            end++;
        }
    }

    fprintf(output, ")");
}


/* Remove a record, if exists */

void removeRecord(BPlusTree* tree, Record key) {
    if(!tree->root) {
        fprintf(stderr, "[B+TREE] Cannot remove into NULL tree reference");
        exit(1);
    }

    Node* cursor = tree->root;
    while(!cursor->isLeaf) { // While a leaf isn't reached
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

    int minKeys = ORDER - 1;
    if(cursor->keysNumber > minKeys) { // A node must containt at least ORDER - 1 keys
        int index;
        for(index = 0; index < cursor->keysNumber && key != cursor->keys[index]; index++);
        if(index == cursor->keysNumber) {
            fprintf(stderr, "[B+TREE] Record was not found %d\n", key);
            return;
        }
        int begin;
        for(begin = index; begin < cursor->keysNumber; begin++) {
            cursor->keys[begin] = cursor->keys[begin + 1]; // Shift left one space
        }
        cursor->keysNumber--;
        cursor->pointers[cursor->keysNumber] = cursor->pointers[cursor->keysNumber + 1];
        cursor->pointers[cursor->keysNumber + 1] = NULL;
    } else {

    }
}

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

/* Create empty BPlus tree
@returns: a new empty tree */

BPlusTree* createTree() {
    BPlusTree* tree = NULL;
    if(!(tree = (BPlusTree*) calloc(1, sizeof(BPlusTree)))) {
        fprintf(stderr, "[B+TREE] Tree initialization failed");
        exit(1);
    }
    tree->root = createNode(true);
    return tree;
}

/* Performs insertion when internal node is not full or split it otherwhise
@params: BPlusTree* tree -> the tree contaning root node, Node* cursor -> the
internal node being verified, Node* child -> child node, Record key -> record
to be inserted. */

void insertInternal(BPlusTree* tree, Node* cursor, Node* child, Record key) {
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
            tree->root = newRoot;
        } else {
            insertInternal(tree, cursor->parent, newNode, cursor->keys[cursor->keysNumber]);
        }
    }

}

/* Insert a record in the tree 
@params: Node* root -> B+ tree's root, Record key -> record to be inserted
@returns: boolean that indicates if op succeded */

void insertRecord(BPlusTree* tree, Record key) {
    if(!tree->root) { // If given tree is invalid
        fprintf(stderr, "[B+TREE] Cannot insert into NULL tree reference");
        exit(1);
    }

    Node* cursor = tree->root;
    //Node* parent = NULL;

    while(!cursor->isLeaf) { // While a leaf isn't reached
        //parent = cursor;
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
            tree->root = newRoot;
        } else { // Hardest of hardest... Splitting time.
            insertInternal(tree, cursor->parent, newLeaf, newLeaf->keys[0]);
        }

    }
}
