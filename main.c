#include "9cc.h"

Token* token;
char* user_input;
Node* code[100];

int main(int argc, char** argv){
  if(argc!= 2){
    fprintf(stderr, "argc != 2");
    return 1;
  }
  
  user_input = argv[1]; 
  token = tokenize(argv[1]);
  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  
  //prologue
  //preapre memory for 26 variants
  printf("	push rbp\n");
  printf("	mov rbp, rsp\n");
  printf("	sub rsp, 208\n");
  
  for (int i = 0; code[i]; i++) {
    gen(code[i]);
    printf("	pop rax\n");
  }
  
  //epilogue  
  printf("	mov rsp, rbp\n");
  printf("	pop rbp\n");
  printf("	ret\n");

  return 0;
}





