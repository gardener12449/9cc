#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

Token* token;
char* user_input;

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





