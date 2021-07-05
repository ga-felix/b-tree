#include <stdbool.h>
#define MIN_ORDER 3

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
    bool isLeaf;
    int keysNumber;
    Record* keys; /* Array of keys */
} Node;

/* Returns initialized B+ Tree's root node */

Node* initializeTree();

/* Print the tree. Check "enunciado.pdf" to
further instructions (there are many!) */

bool printTree(Node* tree);

/* Insert a record */

bool insertRecord(Node* tree, Record key);

/* Remove a record, if exists */

bool removeRecord(Node* tree, Record key);