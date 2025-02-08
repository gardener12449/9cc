#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Token Token;
typedef struct Node Node;

typedef enum {
  TK_RESERVED, //symbol
  TK_IDENT,    //identifier
  TK_NUM,      //integer
  TK_EOF,       //end of input
} Tokenkind;

struct Token {
  Tokenkind kind; //token type
  Token* next;    //next token
  int val;        //numeric value, if kind is int
  char* str;      //token's string
  int len;
};

//variety of nodes of abstract syntax tree
typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
  ND_EQ,
  ND_NEQ,
  ND_GT,
  ND_LT,
  ND_GTE,
  ND_LTE,
  ND_LVAR,
  ND_ASSIGN,
} Nodekind;

//type of nodes of abstract syntax tree
struct Node {
  Nodekind kind; //type of a node
  Node* lhs;     //left hand side
  Node* rhs;     //ringht hand side
  int val;       //nuber, only if kind is ND_NUM
  int offset;    //offset for RBP, only if kind is ND_LVAR
};

extern Token* token;
extern char* user_input;
extern Node* code[100];

void error(char* fmt, ...);
Node* expr();
bool consume(char* p);
Token* consume_ident();
void expect(char* p);
bool at_eof();
int expect_number();
Token* tokenize(char* p);
void program();
void gen(Node* node);

