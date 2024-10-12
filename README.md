# **SimpleLang Parser and AST Generator**

## **Introduction**

This documentation describes the design and implementation of a parser for **SimpleLang**, a minimalistic high-level language for an 8-bit CPU. The parser reads source code written in SimpleLang, constructs an **Abstract Syntax Tree (AST)**, and provides the structure necessary for code generation. The AST represents the hierarchical structure of expressions, assignments, and conditionals in SimpleLang.

The primary features of **SimpleLang** include:
- Variable declaration
- Assignment
- Arithmetic operations (`+`, `-`)
- Conditional statements (`if`)
  
## **Overview**

The parser works in several stages:
1. **Lexical Analysis**: Converts the input source code into tokens using a lexer.
2. **Parsing**: Constructs an AST from the sequence of tokens.
3. **AST Generation**: The parser builds the AST that represents the syntactic structure of the program.
4. **AST Traversal**: This AST can be used later for generating code, analyzing the program, or other compiler phases.

---

## **Token Definitions**

Tokens are the basic building blocks of the parser. Each token represents a meaningful element in SimpleLang, such as keywords (`int`, `if`), operators (`=`, `+`, `-`), and identifiers.

### **Token Types**

| Token Type        | Description                              |
|-------------------|------------------------------------------|
| `TOKEN_INT`       | Represents the `int` keyword             |
| `TOKEN_IDENTIFIER`| Represents variable names or identifiers |
| `TOKEN_NUMBER`    | Represents integer numbers               |
| `TOKEN_ASSIGN`    | Represents the `=` assignment operator   |
| `TOKEN_PLUS`      | Represents the `+` operator              |
| `TOKEN_MINUS`     | Represents the `-` operator              |
| `TOKEN_IF`        | Represents the `if` keyword              |
| `TOKEN_EQUAL`     | Represents the `==` equality operator    |
| `TOKEN_LBRACE`    | Represents `{`                           |
| `TOKEN_RBRACE`    | Represents `}`                           |
| `TOKEN_SEMICOLON` | Represents `;`                           |
| `TOKEN_EOF`       | End of the file/input                    |

---

## **Abstract Syntax Tree (AST)**

The AST is a hierarchical tree representation of the source code, where:
- **Leaf nodes** represent variables or constants.
- **Internal nodes** represent operators or control structures (e.g., assignment, conditional statements).

### **AST Node Structure**

Each node in the AST contains:
- **Type**: The type of token the node represents.
- **Value**: The actual value of the node (such as a variable name or operator).
- **Left** and **Right**: Pointers to child nodes representing sub-expressions or statements.

### **Node Structure in C**

```c
typedef struct Node {
    TokenType type;      // The type of the node (operator, identifier, etc.)
    struct Node* left;   // Left child (for binary operations)
    struct Node* right;  // Right child (for binary operations)
    char value[MAX_TOKEN_LEN];  // The actual value of the token (variable name, number, operator)
} Node;
```

### **AST Example**

For the expression `a = b + 5`, the corresponding AST would look like this:

```
  =
 / \
a   +
   / \
  b   5
```

---

## **Parsing Process**

The parser recursively processes the SimpleLang source code by breaking down each part of the program into its constituent tokens and building an AST that represents the program’s structure.

### **Parsing Functions**

Each major component of SimpleLang has a corresponding parsing function that constructs the AST for that component.

1. **Expressions**: Handles arithmetic expressions such as `a + b`.
2. **Assignments**: Handles assignment operations like `a = 5`.
3. **Conditionals**: Handles conditional statements like `if (a == 5) { ... }`.

### **Recursive Descent Parsing**

The parser uses a **recursive descent parsing** technique, which involves a series of functions that recursively process the input tokens and return AST nodes.

---

### **1. Parsing Expressions**

The `parseExpression` function processes arithmetic expressions and returns an AST node. It recursively parses the operands and operators, constructing a subtree for each binary operation.

```c
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
```

### **2. Parsing Assignments**

The `parseAssignment` function processes assignment statements. It returns an AST node where the left child represents the variable being assigned, and the right child represents the expression being assigned to the variable.

```c
Node* parseAssignment(FILE *file) {
    Node* node = createNode(currentToken);
    match(TOKEN_IDENTIFIER, file);
    match(TOKEN_ASSIGN, file);
    node->right = parseExpression(file);
    match(TOKEN_SEMICOLON, file);
    return node;
}
```

### **3. Parsing Conditionals**

The `parseConditional` function processes `if` statements. It returns a node representing the condition, with left and right children representing the condition and the statement block inside the `if`.

```c
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
```

### **4. Parsing a Program**

The `parseProgram` function processes an entire SimpleLang program by reading each statement and building the AST for it. It continues until the end of the file (`TOKEN_EOF`).

```c
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
```

---

## **AST Traversal and Debugging**

To verify the correctness of the AST, a simple function is used to print the tree structure. This function recursively traverses the AST and prints the nodes with indentation to reflect the hierarchical structure.

### **AST Printing Function**

```c
void printAST(Node* node, int indent) {
    if (node == NULL) return;
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    printf("%s\n", node->value);
    printAST(node->left, indent + 1);
    printAST(node->right, indent + 1);
}
```

### **Example Output for AST**

For the following code:
```c
int a;
a = 10 + 20;
if (a == 30) {
    a = a + 1;
}
```

The AST will look like this:
```
a
  =
    10
    +
      20
if
  ==
    a
    30
  a
    =
      a
      +
        1
```

---

## **Conclusion**

This SimpleLang parser constructs an Abstract Syntax Tree (AST) from SimpleLang programs. The AST represents the structure and relationships between different program constructs like expressions, assignments, and conditionals. The AST is essential for further compiler phases like semantic analysis, optimization, and code generation. The parser was implemented using recursive descent parsing and tested with various SimpleLang programs.
