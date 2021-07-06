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

void readCommands(Node* tree, FILE* input) {

    if (!input) {
        fprintf(stderr, "[READ] Input file stream is NULL (file missing?)");
        exit(1);
    }

    char* line = NULL;
    size_t length = 0;
    ssize_t read;
    int value = 0;
    char command;
    while ((read = getline(&line, &length, input)) != EOF) {
        //fprintf(stdout, "%s", line);
        command = line[0];
        if (command == 'i') { /* Insert record operation */
            chopLine(line);
            value = atoi(line);
            insertRecord(tree, value);
            for(int i = 0; i < tree->keysNumber; i++) {
                printf("%d ", tree->keys[i]);
            }
            printf("\n");
        }

        if (command == 'r') { /* Remove record operation */
            chopLine(line);
            value = atoi(line);
        }
        
        if (command == 'p') { /* Print tree operation */
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
    Node* root = initializeNode();
    readCommands(root, input);
    exit(0);
}