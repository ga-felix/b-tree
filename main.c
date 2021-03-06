#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

/* Chop a line to catch integer argument.
NEVER INCREMENT THE BUFFER POINTER: DANGER OP! */

void chopLine(char* line) {

    if (!line) {
        fprintf(stderr, "[READ] Cannot chop empty line");
        exit(1);
    }

    size_t length = strlen(line);
    memmove(line, line + 1, length);
}

/* Read input file and triggers every command described */

void readCommands(BPlusTree* tree, FILE* input, FILE* output) {

    if (!input || !output) {
        fprintf(stderr, "[READ] Input / output file stream is NULL (file missing?)");
        exit(1);
    }

    char* line = NULL;
    size_t length = 0;
    ssize_t read;
    Record value;
    char command;
    while ((read = getline(&line, &length, input)) != EOF) {
        printf("%s", line);
        command = line[0];
        if (command == 'i') { /* Insert record operation */
            chopLine(line);
            value = atoi(line);
            insertRecord(tree, value);
        }

        if (command == 'r') { /* Remove record operation */
            chopLine(line);
            value = atoi(line);
            removeRecord(tree, value);
        }
        
        if (command == 'p') { /* Print tree operation */
            printTree(tree->root, output);
            fprintf(output, "\n");
        }

        if (command == 'f') { /* Stop input reading */
            exit(0);
        }
    }
    fclose(input);
}

/* Software's entry point */

int main(int argc, char *argv[]) {

    if(argc != 3) {
        fprintf(stderr, "[MAIN] Invalid arguments");
        exit(1);
    }

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w+");
    BPlusTree* tree = createTree();
    readCommands(tree, input, output);
    exit(0);
}