#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

typedef enum {
  TK_RESERVED, //symbol
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
} Nodekind;

//type of nodes of abstract syntax tree
struct Node {
  Nodekind kind; //type of a node
  Node* lhs;     //left hand side
  Node* rhs;     //ringht hand side
  int val;       //nuber, if kind is ND_NUM
};  

//error output
void error(char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

//error output
void error_at(char* loc, char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

//----------------------------------------------------------
//syntax processor
//----------------------------------------------------------
//if the next token is the same as what you expect,
//then return true. in any other case, return false.
bool consume(char* op) {
  if (token->kind != TK_RESERVED ||
      token->len != strlen(op) || 
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

//if the next token is the same as what you expect,
//reads the next token. in any other case, call error()
void expect(char* op) {
  if (token->kind != TK_RESERVED ||
      token->len != strlen(op) || 
      memcmp(token->str, op, token->len))
    error("not %c",op);
  token = token->next;
}

//if the next token is integer, return the value,
//and reads the next token. in any other case, call error()
int expect_number() {
  if (token->kind != TK_NUM)
    error("nor a number");

  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

//make a new token and join it
Token* new_token(Tokenkind kind, Token* cur, char* str, int len) {
  Token* tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

//tokenize the user input and retruns it 
Token* tokenize(char* p) {
  Token head;
  head.next = NULL;
  Token* cur = &head;
  
  while (*p) {
    
    if (isspace(*p)) {
      p++;
      continue;
    }
    
    if (memcmp("==", p, 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (memcmp("!=", p, 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    
    if (memcmp(">=", p, 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
      
    if (memcmp("<=", p, 2) == 0) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }
    
    if (*p == '+' || *p == '-' || *p == '*' ||
          *p == '/' || *p == '(' || *p == ')' ||
	    *p == '>' || *p == '<') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }
          
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("cannot tokenize");
  }
  
  new_token(TK_EOF, cur, p, 1);
  return head.next;
}


