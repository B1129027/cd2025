#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ===== Token 定義 =====
enum TokenType {
    TYPE_TOKEN, MAIN_TOKEN, IF_TOKEN, ELSE_TOKEN, WHILE_TOKEN,
    ID_TOKEN, LITERAL_TOKEN,
    EQUAL_TOKEN, NOTEQUAL_TOKEN,
    GREATER_TOKEN, GREATEREQUAL_TOKEN,
    LESS_TOKEN, LESSEQUAL_TOKEN,
    PLUS_TOKEN, MINUS_TOKEN, ASSIGN_TOKEN,
    LEFTPAREN_TOKEN, RIGHTPAREN_TOKEN,
    LEFTBRACE_TOKEN, RIGHTBRACE_TOKEN,
    SEMICOLON_TOKEN,
    UNKNOWN_TOKEN
};

const char* token_names[] = {
    "TYPE_TOKEN", "MAIN_TOKEN", "IF_TOKEN", "ELSE_TOKEN", "WHILE_TOKEN",
    "ID_TOKEN", "LITERAL_TOKEN",
    "EQUAL_TOKEN", "NOTEQUAL_TOKEN",
    "GREATER_TOKEN", "GREATEREQUAL_TOKEN",
    "LESS_TOKEN", "LESSEQUAL_TOKEN",
    "PLUS_TOKEN", "MINUS_TOKEN", "ASSIGN_TOKEN",
    "LEFTPAREN_TOKEN", "RIGHTPAREN_TOKEN",
    "LEFTBRACE_TOKEN", "RIGHTBRACE_TOKEN",
    "SEMICOLON_TOKEN",
    "UNKNOWN_TOKEN"
};

// ===== linked list 結構 =====
typedef struct TokenNode {
    enum TokenType token;
    int count;
    struct TokenNode *next;
} TokenNode;

void insert_token(TokenNode **head, enum TokenType token) {
    TokenNode *current = *head;
    TokenNode *prev = NULL;

    while (current) {
        if (current->token == token) {
            current->count++;
            return;
        }
        prev = current;
        current = current->next;
    }

    TokenNode *newNode = (TokenNode *)malloc(sizeof(TokenNode));
    newNode->token = token;
    newNode->count = 1;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        prev->next = newNode;
    }
}

void print_token_list(TokenNode *head) {
    TokenNode *current = head;
    while (current) {
        printf("%s : %d\n", token_names[current->token], current->count);
        current = current->next;
    }
}

void free_token_list(TokenNode *head) {
    while (head) {
        TokenNode *temp = head;
        head = head->next;
        free(temp);
    }
}

// ===== Helper =====
int is_keyword(const char* str) {
    if (strcmp(str, "int") == 0) return TYPE_TOKEN;
    if (strcmp(str, "main") == 0) return MAIN_TOKEN;
    if (strcmp(str, "if") == 0) return IF_TOKEN;
    if (strcmp(str, "else") == 0) return ELSE_TOKEN;
    if (strcmp(str, "while") == 0) return WHILE_TOKEN;
    return -1;
}

// ===== DFA 掃描器 =====
void scan_dfa(FILE *file, TokenNode **token_head) {
    enum { STATE_START, STATE_ID, STATE_NUMBER, STATE_ASSIGN, STATE_NOT, STATE_GREATER, STATE_LESS } state = STATE_START;
    char buffer[128];
    int idx = 0;
    int c;

    while ((c = fgetc(file)) != EOF) {
        switch (state) {
            case STATE_START:
                if (isspace(c)) continue;

                idx = 0;

                if (isalpha(c) || c == '_') {
                    buffer[idx++] = c;
                    state = STATE_ID;
                }
                else if (isdigit(c)) {
                    buffer[idx++] = c;
                    state = STATE_NUMBER;
                }
                else {
                    switch (c) {
                        case '=': state = STATE_ASSIGN; break;
                        case '!': state = STATE_NOT; break;
                        case '>': state = STATE_GREATER; break;
                        case '<': state = STATE_LESS; break;
                        case '+': insert_token(token_head, PLUS_TOKEN); break;
                        case '-': insert_token(token_head, MINUS_TOKEN); break;
                        case '(': insert_token(token_head, LEFTPAREN_TOKEN); break;
                        case ')': insert_token(token_head, RIGHTPAREN_TOKEN); break;
                        case '{': insert_token(token_head, LEFTBRACE_TOKEN); break;
                        case '}': insert_token(token_head, RIGHTBRACE_TOKEN); break;
                        case ';': insert_token(token_head, SEMICOLON_TOKEN); break;
                        default:  insert_token(token_head, UNKNOWN_TOKEN); break;
                    }
                }
                break;

            case STATE_ID:
                if (isalnum(c) || c == '_') {
                    buffer[idx++] = c;
                } else {
                    buffer[idx] = '\0';
                    ungetc(c, file);
                    int kw = is_keyword(buffer);
                    if (kw != -1)
                        insert_token(token_head, kw);
                    else
                        insert_token(token_head, ID_TOKEN);
                    state = STATE_START;
                }
                break;

            case STATE_NUMBER:
                if (isdigit(c)) {
                    buffer[idx++] = c;
                } else {
                    buffer[idx] = '\0';
                    ungetc(c, file);
                    insert_token(token_head, LITERAL_TOKEN);
                    state = STATE_START;
                }
                break;

            case STATE_ASSIGN:
                if (c == '=') insert_token(token_head, EQUAL_TOKEN);
                else {
                    ungetc(c, file);
                    insert_token(token_head, ASSIGN_TOKEN);
                }
                state = STATE_START;
                break;

            case STATE_NOT:
                if (c == '=') insert_token(token_head, NOTEQUAL_TOKEN);
                else {
                    ungetc(c, file);
                    insert_token(token_head, UNKNOWN_TOKEN);
                }
                state = STATE_START;
                break;

            case STATE_GREATER:
                if (c == '=') insert_token(token_head, GREATEREQUAL_TOKEN);
                else {
                    ungetc(c, file);
                    insert_token(token_head, GREATER_TOKEN);
                }
                state = STATE_START;
                break;

            case STATE_LESS:
                if (c == '=') insert_token(token_head, LESSEQUAL_TOKEN);
                else {
                    ungetc(c, file);
                    insert_token(token_head, LESS_TOKEN);
                }
                state = STATE_START;
                break;
        }
    }
}

// ===== main =====
int main() {
    FILE *file = fopen("main.c", "r");  // 掃描 main.c
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    TokenNode *token_list = NULL;
    scan_dfa(file, &token_list);
    fclose(file);

    print_token_list(token_list);
    free_token_list(token_list);
    return 0;
}
