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

//----------------------------------------------------------
//node processer
//----------------------------------------------------------
Node* new_node(Nodekind kind, Node* lhs, Node* rhs) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node* new_node_num(int val) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_NUM; 
  node->val = val;
  return node;
}

Node* primary() {
  if (consume("(")) {
    Node* node = expr();
    expect(")");
    return node;
  }
  return new_node_num(expect_number());
}

Node* unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node* mul() {
  Node* node = unary();
    
  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node* add() {
  Node* node = mul();

  for (;;) {
    if(consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
   }
}

Node* relational() {
  Node* node = add();

  if (consume(">="))
    node = new_node(ND_GTE, node, add());
    
  else if (consume("<="))
    node = new_node(ND_LTE, node, add());
  
  else if (consume(">"))
    node = new_node(ND_GT, node, add());

  else if (consume("<"))
    node = new_node(ND_LT, node, add());

  return node;
}
   
Node* equality() {
  Node* node = relational();

  if (consume("=="))
    node = new_node(ND_EQ, node, relational());
  
  else if (consume("!="))
    node = new_node(ND_NEQ, node, relational());

  return node;
}  

Node* expr() { return equality(); }

void gen(Node* node) {
  if (node->kind == ND_NUM) {
    printf("	push %d\n", node->val);
    return;
  }
  
  gen(node->lhs);
  gen(node->rhs);
  
  printf("	pop rdi\n");
  printf("	pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("	add rax, rdi\n");
      break;
    case ND_SUB:
      printf("	sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("	imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("	cqo\n");
      printf("	idiv rdi\n");
      break;
    case ND_EQ:
      printf("	cmp rax, rdi\n");
      printf("	sete al\n");
      printf("	movzb rax, al\n");
      break;
    case ND_NEQ:
      printf("	cmp rax, rdi\n");
      printf("	setne al\n");
      printf("	movzb rax, al\n");
      break;
    case ND_GT:
      printf("	cmp rax, rdi\n");
      printf("	setg al\n");
      printf("	movzb rax, al\n");
      break;
    case ND_LT:
      printf("	cmp rax, rdi\n");
      printf("	setl al\n");
      printf("	movzb rax, al\n");
      break;
    case ND_GTE:
      printf("	cmp rax, rdi\n");
      printf("	setge al\n");
      printf("	movzb rax, al\n");
      break;
    case ND_LTE:
      printf("	cmp rax, rdi\n");
      printf("	setle al\n");
      printf("	movzb rax, al\n");
      break;
    }

  printf("	push rax\n");
}

