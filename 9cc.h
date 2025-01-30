typedef struct Token Token;
typedef struct Node Node;

extern Token* token;
extern char* user_input;

Node* expr();
bool consume(char* p);
void expect(char* p);
int expect_number();
Token* tokenize(char* p);
void gen(Node* node);

