#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char ch;
    int count;
    struct Node *next;
} Node;

void insert(Node **head, char c) {
    Node *current = *head;
    Node *prev = NULL;

    while (current) {
        if (current->ch == c) {
            current->count++;
            return;
        }
        prev = current;
        current = current->next;
    }

    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->ch = c;
    newNode->count = 1;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        prev->next = newNode;
    }
}


// Free the linked list
void freeList(Node *head) {
    while (head) {
        Node *temp = head;
        head = head->next;
        free(temp);
    }
}

// Print the linked list
void printList(Node *head) {
    Node *current = head;
    while (current) {
        printf("%c : %d\n", current->ch, current->count);
        current = current->next;
    }
}

int main() {
    FILE *file = fopen("main.c", "r"); // Make sure input.txt exists
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    Node *head = NULL;
    char c;

    // Read file contents and store in linked list
    while ((c = fgetc(file)) != EOF) {
        insert(&head, c);
    }

    fclose(file);

    // Print the result
    printList(head);

    // Free allocated memory
    freeList(head);

    return 0;
}

