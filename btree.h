#include <stdbool.h>
#define ORDER 3
#define MAX ORDER * 2
#define MAXKEYS MAX - 1

/* Sets type of records the tree should storage */

typedef int Record;

/* Defines B+ Tree's structure. Keep in mind that:
1 - pointers must be void** as it can storage either
pointers to Record or Node.
2 - the exercise specifies that keys must be kept
at leaf node level only. */

typedef struct Node {
    struct Node* parent;
    void** pointers;
    Record* keys;
    bool isLeaf;
    int keysNumber;
} Node;

typedef struct {
    Node* root;
} BPlusTree;

BPlusTree* createTree();

/* Print the tree. Check "enunciado.pdf" to
further instructions (there are many!) */

void printTree(Node* cursor, FILE* output);

/* Insert a record */

bool insertRecord(BPlusTree* tree, Record key);

/* Remove a record, if exists */

bool removeRecord(BPlusTree* tree, Record key);