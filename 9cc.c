#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  TK_RESERVED, //symbol
  TK_NUM,      //integer
  TK_EOF,       //end of input
} Tokenkind;

typedef struct Token Token;

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

typedef struct Node Node;

//type of nodes of abstract syntax tree
struct Node {
  Nodekind kind; //type of a node
  Node* lhs;     //left hand side
  Node* rhs;     //ringht hand side
  int val;       //nuber, if kind is ND_NUM
};  

Token* token; //current token
char* user_input;

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

Node* expr();

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

int main(int argc, char** argv){
  if(argc!= 2){
    fprintf(stderr, "argc != 2");
    return 1;
  }

  token = tokenize(argv[1]);
  Node* node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  
  gen(node);  
  printf("	pop rax\n");
  printf("	ret\n");

  /*token only*/
  /*printf("  mov rax, %d\n", expect_number());
  
  while (!at_eof()){
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");*/


  return 0;
}





