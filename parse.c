#include "9cc.h"

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

Token* consume_ident() {
  if (token->kind != TK_IDENT ||
      token->str[0] < 'a' || token->str[0] > 'z')
    return false;
  Token* tok = token;
  token = token->next;
  return tok;
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
	    *p == '>' || *p == '<' || *p == ';' ||
	      *p == '=') {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      cur = new_token(TK_IDENT, cur, p++, 1);
      continue;
    }
          
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 1);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("cannot tokenize, %s", *p);
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

Node* primary() {
  if (consume("(")) {
    Node* node = expr();
    expect(")");
    return node;
  }

  Token* tok = consume_ident();
  if (tok) {
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    node->offset = (tok->str[0] - 'a' + 1) * 8;
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

    else return node;
   }
}

Node* relational() {
  Node* node = add();

  for (;;) {
    if (consume(">="))
      node = new_node(ND_GTE, node, add());
    else if (consume("<="))
      node = new_node(ND_LTE, node, add());
    else if (consume(">"))
      node = new_node(ND_GT, node, add());
    else if (consume("<"))
      node = new_node(ND_LT, node, add());

    else return node;
  }

}

Node* equality() {
  Node* node = relational();

  if (consume("=="))
    node = new_node(ND_EQ, node, relational());
  else if (consume("!="))
    node = new_node(ND_NEQ, node, relational());

  return node;
}

Node* assign() {
  Node* node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node* expr() { return assign(); }

Node* stmt() {
  Node* node = expr();
  expect(";");
  return node;
}

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}
