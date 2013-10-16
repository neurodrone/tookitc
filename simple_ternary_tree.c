/**
 * @author Vaibhav Bhembre
 * @version 2013/10/14
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

struct Node {
    struct Node *l;
    struct Node *c;
    struct Node *r;
    char data;
    bool string:1; /* 1 bit is more than enough, no biggie though as Node is not packed */
};

struct Node *new_node(char letter) {
    struct Node *node = malloc(sizeof (*node));
    assert(node != NULL);

    node->l = node->c = node->r = NULL;
    node->data = letter;
    node->string = 0;

    return node;
}

void insert(struct Node **root, char *letter) {
    if (!*root) {
        *root = new_node(*letter);
    }

    if (*letter < (*root)->data) {
        insert(&((*root)->l), letter);
    } else if (*letter > (*root)->data) {
        insert(&((*root)->r), letter);
    } else {
        if (*(letter + 1)) {
            insert(&((*root)->c), letter + 1);
        } else {
            (*root)->string = 1;
        }
    }
}

bool search(struct Node *root, char *word) {
    if (!root) return 0;

    if (*word < root->data) {
        search(root->l, word);
    } else if (*word > root->data) {
        search(root->r, word);
    } else {
        if (*(word + 1) == '\0') {
            return root->string;
        }
        return search(root->c, word + 1);
    }
}

int main(int argc, char *argv[]) {
    struct Node *root = NULL;
    insert(&root, "rhino");
    insert(&root, "rhinos");

    char *cmp = "rhinos";
    printf("%s word: %s\n", search(root, cmp) ? "FOUND" : "NOT FOUND", cmp);

    cmp = "dog";
    printf("%s word: %s\n", search(root, cmp) ? "FOUND" : "NOT FOUND", cmp);

    cmp = "rhianna";
    printf("%s word: %s\n", search(root, cmp) ? "FOUND" : "NOT FOUND", cmp);
    return 0;
}

