#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  TK_RESERVED, //記号
  TK_NUM,      //整数トークン
  TK_EOF      //入力の終わりのトークン
} Tokenkind;

typedef struct Token Token;

struct Token {
  Tokenkind kind; //token type
  Token* next;    //next token
  int val;        //numeric value, if kind is int
  char* str;      //token's string
};

Token* token; //current token
char* user_input;

//エラー出力
void error(char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

//エラー出力２
void error_at(char* loc, char* fmt, ...){
  va_list ap;
  va_start(ap, fmt);

  int pos = 

//次のトークンが期待している記号ならトークンを無為に
//読み進めて真を返す。それ以外は偽を返す
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

//次のトークンが期待している記号ならトークンを読み進める。
//それ以外の場合はエラーを報告する
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error("not %c",op);
  token = token->next;
}

//次のトークンが数値ならトークンを読み進め数値を返す
//それ以外の場合はエラーを報告する
int expect_number() {
  if (token->kind != TK_NUM)
    error("not a number");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

//新しいトークンを作成してつなげる
Token* new_token(Tokenkind kind, Token* cur, char* str) {
  Token* tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

//入力文字列をトークナイズして返す
Token* tokenize(char* p) {
  Token head;
  head.next = NULL;
  Token* cur = &head;
  
  while (*p) {
    
    if (isspace(*p)) {
      p++;
      continue;
    }
    
    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("cannot tokenize");
  }
  
  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char** argv){
  if(argc!= 2){
    fprintf(stderr, "argc != 2");
    return 1;
  }

  token = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("  mov rax, %d\n", expect_number());
  
  while (!at_eof()){
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  return 0;
}





