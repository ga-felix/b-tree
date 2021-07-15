#include <stdio.h>
#include <stdlib.h>
#include "btree.h"
        /*for(signal = cursor->keysNumber; signal > index; signal--) {
            cursor->keys[signal] = cursor->keys[signal - 1];
        }
        for(signal = cursor->keysNumber + 1; signal > index + 1; signal--) {
            cursor->pointers[signal] = cursor->pointers[signal - 1];
        }*/
void shiftPointers(Node* node, int from, int to, bool right) {
    int begin;
    if(right) {
        for(begin = from; begin > to; begin--) {
            node->pointers[begin] = node->pointers[begin - 1];
        }
    } else {
        for(begin = from; begin < to; begin++) {
            node->pointers[begin] = node->pointers[begin + 1];
        }
    }
}

void shiftKeys(Node* node, int from, int to, bool right) {
    int begin;
    if(right) {
        for(begin = from; begin > to; begin--) {
            node->keys[begin] = node->keys[begin - 1];
        }
    } else {
        for(begin = from; begin < to; begin++) {
            node->keys[begin] = node->keys[begin + 1];
        }
    }
}

/* Print the tree. Check "enunciado.pdf" to
further instructions (there are many!) 
@params: Node* cursor -> It must be root level node, FILE* output -> File to write output */

void printTree(Node* cursor, FILE* output) {
    if(!cursor->pointers[0] && !cursor->parent && cursor->keys[0] == 0) {
        fprintf(output, "Vazia");
        return;
    }
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
    int signal;
    Node* child;
    for(index = 0, signal = 0; index < cursor->keysNumber + 1; index++) {
        child = (Node*) cursor->pointers[index];
        printTree(child, output);
        if(signal < cursor->keysNumber) {
            fprintf(output, " %d ", cursor->keys[signal]);
            signal++;
        }
    }

    fprintf(output, ")");
}


/* Frees node's memory
@params: Node* node -> node to have memory freed */

void destroyNode(Node* node) {
    free(node->keys);
    free(node->pointers);
    free(node);
}

/* Performs internal's node removal
@params: BPlusTree* tree -> tree to be altered, Node* cursor -> navigation pointer,
Node* child -> auxiliar navigation pointer, Record key -> the key to be removed */

void removeInternal(BPlusTree* tree, Node* cursor, Node* child, Record key) { // Reviewed
    if(!cursor->parent) {
        if(cursor->keysNumber == 1) {
            if(cursor->pointers[1] == child) {
                destroyNode(child);
                tree->root = cursor->pointers[0];
                return;
            } else if(cursor->pointers[0] == child) {
                destroyNode(child);
                tree->root = cursor->pointers[1];
                return;
            }
            destroyNode(cursor);
        }
    }
    int marker, index;
    for(marker = 0; marker < cursor->keysNumber; marker++) {
        if(cursor->keys[marker] == key) {
            break;
        }
    }
    for(index = marker; index <  cursor->keysNumber; index++) {
        cursor->keys[index] = cursor->keys[index + 1]; 
    }
    for(marker = 0; marker < cursor->keysNumber + 1; marker++) {
        if(cursor->pointers[marker] == child) {
            break;
        }
    }
    for(index = marker; index < cursor->keysNumber + 1; index++) {
        cursor->pointers[index] = cursor->pointers[index + 1]; 
    }
    cursor->keysNumber--;
    if(cursor->keysNumber < ORDER - 1) {
        if(!cursor->parent) return;
        int left, right;
        for(marker = 0; marker < cursor->parent->keysNumber + 1; marker++) {
            if(cursor->parent->pointers[marker] == cursor) {
                left = marker - 1;
                right = marker + 1;
                break;
            }
        }
        Node* neighboor = cursor->parent->pointers[left];
        if(left >= 0 && neighboor->keysNumber > ORDER - 1) {
            for(index = cursor->keysNumber; index > 0; index--) {
                cursor->keys[index] = cursor->keys[index - 1];
            }
            cursor->keys[0] = cursor->parent->keys[left];
            cursor->parent->keys[left] = neighboor->keys[neighboor->keysNumber - 1];
            for(index = cursor->keysNumber + 1; index > 0; index--) {
                cursor->pointers[index] = cursor->pointers[index - 1];
            }
            cursor->pointers[0] = neighboor->pointers[neighboor->keysNumber];
            cursor->keysNumber++;
            neighboor->keysNumber--;
            return;
        }

        neighboor = cursor->parent->pointers[right];
        if(right <= cursor->parent->keysNumber && neighboor->keysNumber > ORDER - 1) {
            cursor->keys[cursor->keysNumber] = cursor->parent->keys[marker];
            cursor->parent->keys[marker] = neighboor->keys[0];
            for(index = 0; index < neighboor->keysNumber - 1; index++) {
                neighboor->keys[index] = neighboor->keys[index + 1];
            }
            cursor->pointers[cursor->keysNumber + 1] = neighboor->pointers[0];
            for(index = 0; index < neighboor->keysNumber; index++) {
                neighboor->pointers[index] = neighboor->pointers[index + 1];
            }
            cursor->keysNumber++;
            neighboor->keysNumber--;
            return;
        }

        neighboor = cursor->parent->pointers[left];
        if(left >= 0) {
            neighboor->keys[neighboor->keysNumber] = cursor->parent->keys[left];
            for(index = neighboor->keysNumber + 1, marker = 0; marker < cursor->keysNumber; marker++) {
                neighboor->keys[index] = cursor->keys[marker];
            }
            for(index = neighboor->keysNumber + 1, marker = 0; marker < cursor->keysNumber + 1; marker++) {
                neighboor->pointers[index] = cursor->pointers[marker];
                cursor->pointers[marker] = NULL;
            }
            neighboor->keysNumber += cursor->keysNumber + 1;
            cursor->keysNumber = 0;
            removeInternal(tree, cursor->parent, cursor, cursor->parent->keys[left]);
        } else if(right <= cursor->parent->keysNumber) {
            neighboor = cursor->parent->pointers[right];
            cursor->keys[cursor->keysNumber] = cursor->parent->keys[right - 1];
            for(index = cursor->keysNumber + 1, marker = 0; marker < neighboor->keysNumber; marker++) {
                cursor->keys[index] = neighboor->keys[marker];
            }
            for(index = cursor->keysNumber + 1, marker = 0; marker < neighboor->keysNumber + 1; marker++) {
                cursor->pointers[index] = neighboor->pointers[marker];
                neighboor->pointers[marker] = NULL;
            }
            cursor->keysNumber += neighboor->keysNumber + 1;
            neighboor->keysNumber = 0;
            removeInternal(tree, cursor->parent, neighboor, cursor->parent->keys[right - 1]);
        }
    }
}

/* Remove a record, if exists
@params: BPlusTree* tree -> tree to be altered, Record key -> key to be removed
@returns: boolean that indicates if operation succeded */

bool removeRecord(BPlusTree* tree, Record key) { // Reviewed
    if(!tree->root) {
        fprintf(stderr, "[B+TREE] Cannot remove into NULL tree reference");
        exit(1);
    }

    Node* cursor = tree->root;
    int left, right;
    while(!cursor->isLeaf) { // While a leaf isn't reached
        int index;
        for(index = 0; index < cursor->keysNumber; index++) {
            left = index - 1;
            right = index + 1;
            if(key < cursor->keys[index]) {
                cursor = (Node*) cursor->pointers[index];
                break;
            }
            if(index == cursor->keysNumber - 1) {
                left = index;
                right = index + 2;
                cursor = (Node*) cursor->pointers[index + 1]; // Key can be greater than all keys here
                break;
            }
        }
    }
    bool keyExists = false;
    int index;
    for(index = 0; index < cursor->keysNumber; index++) {
        if(cursor->keys[index] == key) {
            keyExists = true;
            break;
        }
    }
    if(!keyExists) {
        fprintf(stderr, "[B+TREE] Key doesn't exists\n");
        return false;
    }

    int marker;
    for(marker = index; marker < cursor->keysNumber; marker++) {
        cursor->keys[marker] = cursor->keys[marker + 1];
    }
    cursor->keysNumber--;
    if(!cursor->parent) {
        for(index = 0; index < cursor->keysNumber + 1; index++) {
            cursor->pointers[index] = NULL;
        }
        return true;
    }
    cursor->pointers[cursor->keysNumber] = cursor->pointers[cursor->keysNumber + 1];
    cursor->pointers[cursor->keysNumber + 1] = NULL;

    if(cursor->keysNumber < ORDER - 1) { // Underflow
        Node* neighboor = cursor->parent->pointers[left];
        if(left >= 0 && neighboor->keysNumber > ORDER - 1) {
            for(index = cursor->keysNumber; index > 0; index--) {
                cursor->keys[index] = cursor->keys[index - 1];
            }
            cursor->keysNumber++;
            cursor->pointers[cursor->keysNumber] = cursor->pointers[cursor->keysNumber - 1];
            cursor->pointers[cursor->keysNumber - 1] = NULL;
            cursor->keys[0] = neighboor->keys[neighboor->keysNumber - 1];
            neighboor->keysNumber--;
            neighboor->pointers[neighboor->keysNumber] = (void*) cursor;
            neighboor->pointers[neighboor->keysNumber + 1] = NULL;
            cursor->parent->keys[left] = cursor->keys[0];
            return true;
        }
        neighboor = cursor->parent->pointers[right];
        if(right <= cursor->parent->keysNumber && neighboor->keysNumber > ORDER - 1) {
            cursor->keysNumber++;
            cursor->pointers[cursor->keysNumber] = cursor->pointers[cursor->keysNumber - 1];
            cursor->pointers[cursor->keysNumber - 1] = NULL;
            cursor->keys[cursor->keysNumber - 1] = neighboor->keys[0];
            neighboor->keysNumber--;
            neighboor->pointers[neighboor->keysNumber] = neighboor->pointers[neighboor->keysNumber + 1];
            neighboor->pointers[neighboor->keysNumber + 1] = NULL;
            for(index = 0; index < neighboor->keysNumber; index++) {
                neighboor->keys[index] = neighboor->keys[index + 1];
            }
            cursor->parent->keys[right - 1] = neighboor->keys[0];
            return true;
        }

        if(left >= 0) {
            neighboor = cursor->parent->pointers[left];
            for(index = neighboor->keysNumber, marker = 0; marker < cursor->keysNumber; index++, marker++) {
                neighboor->keys[index] = cursor->keys[marker];
            }
            neighboor->pointers[neighboor->keysNumber] = NULL;
            neighboor->keysNumber += cursor->keysNumber;
            neighboor->pointers[neighboor->keysNumber] = cursor->pointers[cursor->keysNumber];
            removeInternal(tree, cursor->parent, cursor, cursor->parent->keys[left]);
            destroyNode(cursor);
        } else if(right <= cursor->parent->keysNumber) {
            neighboor = cursor->parent->pointers[right];
            for(index = cursor->keysNumber, marker = 0; marker < neighboor->keysNumber; index++, marker++) {
                cursor->keys[index] = neighboor->keys[marker];
            }
            cursor->pointers[cursor->keysNumber] = NULL;
            cursor->keysNumber += neighboor->keysNumber;
            cursor->pointers[cursor->keysNumber] = neighboor->pointers[neighboor->keysNumber];
            removeInternal(tree, cursor->parent, neighboor, cursor->parent->keys[right - 1]);
            destroyNode(neighboor);
        }
    } else {
        bool hasKey = false;
        for(index = 0; index < cursor->parent->keysNumber; index++) {
            if(cursor->parent->keys[index] == key) {
                hasKey = true;
                break;
            }
        }
        if(hasKey) {
            cursor->parent->keys[index] = cursor->keys[0];
        }
    }
    return true;
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

void insertInternal(BPlusTree* tree, Node* cursor, Node* child, Record key) { // Reviewed
    if(cursor->keysNumber < MAXKEYS) {
        int index;
        for(index = 0; index < cursor->keysNumber && key > cursor->keys[index]; index++);
        shiftKeys(cursor, cursor->keysNumber, index, true);
        shiftPointers(cursor, cursor->keysNumber + 1, index + 1, true);
        cursor->keys[index] = key;
        cursor->keysNumber++;
        cursor->pointers[index + 1] = child;
        child->parent = cursor;
    } else {
        Node* newNode = createNode(false);
        int index, signal;
        int median = (MAXKEYS) / 2;
        Record medianValue = cursor->keys[median];
        for(index = 0, signal = median + 1; index < median + 1; index++, signal++) {
            newNode->keys[index] = cursor->keys[signal];
        }
        for(index = 0, signal = median + 1; index < median + 2; index++, signal++) {
            newNode->pointers[index] = cursor->pointers[signal];
        }
        cursor->keysNumber = median - 0;
        newNode->keysNumber = median + 0;
        if(key >= cursor->keys[cursor->keysNumber]) {
            for(index = 0; index < newNode->keysNumber && key > newNode->keys[index]; index++);
            shiftKeys(newNode, newNode->keysNumber, index, true);
            shiftPointers(newNode, newNode->keysNumber + 1, index + 1, true);
            newNode->keys[index] = key;
            newNode->pointers[index + 1] = (void*) child;
            newNode->keysNumber++;
        } else {
            for(index = 0; index < cursor->keysNumber && key > cursor->keys[index]; index++);
            shiftKeys(cursor, cursor->keysNumber, index, true);
            shiftPointers(cursor, cursor->keysNumber + 1, index + 1, true);
            cursor->keys[index] = key;
            cursor->pointers[index + 1] = (void*) child;
            cursor->keysNumber++;
            cursor->keys[cursor->keysNumber] = medianValue;
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
            insertInternal(tree, cursor->parent, newNode, medianValue);//cursor->keys[cursor->keysNumber]);
        }
    }
}

/* Insert a record in the tree 
@params: Node* root -> B+ tree's root, Record key -> record to be inserted
@returns: boolean that indicates if operation succeded */

bool insertRecord(BPlusTree* tree, Record key) {
    if(!tree->root) {
        fprintf(stderr, "[B+TREE] Cannot insert into NULL tree reference");
        return false;
    }

    Node* cursor = tree->root;
    while(!cursor->isLeaf) {
        int index;
        for(index = 0; index < cursor->keysNumber; index++) {
            if(key < cursor->keys[index]) {
                cursor = (Node*) cursor->pointers[index];
                break;
            }
            if(index == cursor->keysNumber - 1) {
                cursor = (Node*) cursor->pointers[index + 1];
                break;
            }
        }
    }

    if(cursor->keysNumber < MAXKEYS) {
        int index;
        for(index = 0; index < cursor->keysNumber && key > cursor->keys[index]; index++);
        shiftKeys(cursor, cursor->keysNumber, index, true);
        cursor->keys[index] = key;
        cursor->keysNumber++;
        cursor->pointers[cursor->keysNumber] = cursor->pointers[cursor->keysNumber - 1];
        cursor->pointers[cursor->keysNumber - 1] = NULL;
    } else {
        Node* newLeaf = createNode(true);
        int index, signal;
        int median = (MAXKEYS) / 2;
        Record medianValue = cursor->keys[median];
        for(index = 0, signal = median; index < median + 1; index++, signal++) {
            newLeaf->keys[index] = cursor->keys[signal];
        }
        cursor->keysNumber = median;
        newLeaf->keysNumber = median + 1;
        if(key >= medianValue) {
            for(index = 0; index < newLeaf->keysNumber && key > newLeaf->keys[index]; index++);
            shiftKeys(newLeaf, newLeaf->keysNumber, index, true);
            newLeaf->keys[index] = key;
            newLeaf->keysNumber++;
        } else {
            for(index = 0; index < cursor->keysNumber && key > cursor->keys[index]; index++);
            shiftKeys(cursor, cursor->keysNumber, index, true);
            cursor->keys[index] = key;
            cursor->keysNumber++;
        }
        cursor->pointers[cursor->keysNumber] = (void*) newLeaf;
        newLeaf->pointers[newLeaf->keysNumber] = cursor->pointers[MAXKEYS];
        cursor->pointers[MAXKEYS] = NULL;
        if(!cursor->parent) {
            Node* newRoot = createNode(false);
            newRoot->keys[0] = newLeaf->keys[0];
            newRoot->pointers[0] = (void*) cursor;
            newRoot->pointers[1] = (void*) newLeaf;
            newRoot->keysNumber = 1;
            cursor->parent = newRoot;
            newLeaf->parent = newRoot;
            tree->root = newRoot;
        } else {
            insertInternal(tree, cursor->parent, newLeaf, medianValue);//newLeaf->keys[0]);
        }
    }
    return true;
}
