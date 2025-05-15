#include <stdio.h>
#include <string.h>

// ===== Token 定義 =====
typedef enum {
    LITERAL_TOKEN,
    PLUS_TOKEN,
    LEFTPAREN_TOKEN,
    RIGHTPAREN_TOKEN,
    EOF_TOKEN,
    UNKNOWN_TOKEN
} TokenType;

const char *token_names[] = {
    "LITERAL_TOKEN",
    "PLUS_TOKEN",
    "LEFTPAREN_TOKEN",
    "RIGHTPAREN_TOKEN",
    "EOF_TOKEN",
    "UNKNOWN_TOKEN"
};

// ===== Lexer 模擬區（請用 HW#1 改成讀字元分析） =====
char input[256];
int pos = 0;
TokenType token;
char current_char;

// 模擬讀取下一個 token 的邏輯（應從 HW#1 scanner 接進來）
TokenType nextToken() {
    while (input[pos] == ' ') pos++;
    current_char = input[pos++];

    if (current_char >= '0' && current_char <= '9') {
        return LITERAL_TOKEN;
    } else if (current_char == '+') {
        return PLUS_TOKEN;
    } else if (current_char == '(') {
        return LEFTPAREN_TOKEN;
    } else if (current_char == ')') {
        return RIGHTPAREN_TOKEN;
    } else if (current_char == '\0') {
        return EOF_TOKEN;
    } else {
        return UNKNOWN_TOKEN;
    }
}

void throwError() {
    printf("Parse Error at pos %d (token: %s)\n", pos - 1, token_names[token]);
}

// ===== Parser Functions =====

void parse_E();  // 提前宣告

void parse_S() {
    if (token == LITERAL_TOKEN || token == LEFTPAREN_TOKEN) {
        parse_E();
        parse_S1();
    } else {
        throwError();
    }
}

void parse_S1() {
    switch (token) {
        case PLUS_TOKEN:
            token = nextToken();
            parse_S();
            break;
        case RIGHTPAREN_TOKEN:
        case EOF_TOKEN:
            // ε production
            return;
        default:
            throwError();
    }
}

void parse_E() {
    if (token == LITERAL_TOKEN) {
        token = nextToken();
    } else if (token == LEFTPAREN_TOKEN) {
        token = nextToken();
        parse_S();
        if (token != RIGHTPAREN_TOKEN) {
            throwError();
            return;
        }
        token = nextToken();
    } else {
        throwError();
    }
}

// ===== 主程式入口 =====

int main() {
    printf("Enter expression: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0'; // 移除換行字元

    pos = 0;
    token = nextToken();

    parse_S();

    if (token == EOF_TOKEN) {
        printf("Parse Success!\n");
    } else {
        printf("Parse Failed: Extra tokens after parsing.\n");
    }

    return 0;
}
