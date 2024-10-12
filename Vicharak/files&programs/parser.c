#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_LEN 100

typedef enum {
    TOKEN_INT, TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_ASSIGN,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_IF, TOKEN_EQUAL,
    TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_SEMICOLON, TOKEN_EOF, TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char text[MAX_TOKEN_LEN];
} Token;

typedef struct Node {
    TokenType type;
    struct Node* left;
    struct Node* right;
    char value[MAX_TOKEN_LEN];
} Node;

Token currentToken;

void getNextToken(FILE *file) {
}

Node* createNode(Token token) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->type = token.type;
    strcpy(node->value, token.text);
    node->left = node->right = NULL;
    return node;
}

void match(TokenType expected, FILE *file) {
    if (currentToken.type == expected) {
        getNextToken(file);
    } else {
        printf("Syntax error: expected token %d but got %d\n", expected, currentToken.type);
        exit(1);
    }
}

Node* parseFactor(FILE *file) {
    Node* node = createNode(currentToken);
    if (currentToken.type == TOKEN_NUMBER || currentToken.type == TOKEN_IDENTIFIER) {
        match(currentToken.type, file);
    } else {
        printf("Syntax error: expected a number or identifier\n");
        exit(1);
    }
    return node;
}

Node* parseExpression(FILE *file) {
    Node* node = parseFactor(file);
    while (currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS) {
        Token operator = currentToken;
        match(currentToken.type, file);
        Node* operatorNode = createNode(operator);
        operatorNode->left = node;
        operatorNode->right = parseFactor(file);
        node = operatorNode;
    }
    return node;
}

Node* parseAssignment(FILE *file) {
    Node* node = createNode(currentToken);
    match(TOKEN_IDENTIFIER, file);
    match(TOKEN_ASSIGN, file);
    node->right = parseExpression(file);
    match(TOKEN_SEMICOLON, file);
    return node;
}

Node* parseDeclaration(FILE *file) {
    match(TOKEN_INT, file);
    Node* node = createNode(currentToken);
    match(TOKEN_IDENTIFIER, file);
    match(TOKEN_SEMICOLON, file);
    return node;
}

Node* parseConditional(FILE *file) {
    match(TOKEN_IF, file);
    match(TOKEN_LBRACE, file);
    Node* node = createNode(currentToken);
    node->left = parseExpression(file);
    match(TOKEN_EQUAL, file);
    node->right = parseExpression(file);
    match(TOKEN_RBRACE, file);
    match(TOKEN_LBRACE, file);
    Node* body = parseAssignment(file);
    node->left = body;
    match(TOKEN_RBRACE, file);
    return node;
}

void parseProgram(FILE *file) {
    getNextToken(file);
    while (currentToken.type != TOKEN_EOF) {
        Node* stmt;
        if (currentToken.type == TOKEN_INT) {
            stmt = parseDeclaration(file);
        } else if (currentToken.type == TOKEN_IF) {
            stmt = parseConditional(file);
        } else if (currentToken.type == TOKEN_IDENTIFIER) {
            stmt = parseAssignment(file);
        }
        printf("Parsed statement with root: %s\n", stmt->value);
    }
}

int main() {
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }
    parseProgram(file);
    fclose(file);
    return 0;
}
