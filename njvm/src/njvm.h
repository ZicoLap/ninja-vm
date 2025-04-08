#include <stdbool.h>
#include "bigint.h"
#include "support.h"
#include <stdint.h>




void printer(unsigned int IR);
void  executeNVM(const char *filename);
void  help();
void execute(int code[]);
void ausfuehren(unsigned int IR);
void push(int a);
int pop();
void add();
void sub();
void mul();
void divison();
void mod();
void rdint();
void wrint();
void rdchar();
void wrchar();
void halt();
void pushg(int x);
void popg(int x);
void asf(int n);
void rsf();
void pushl(int n);
void popl(int n);
void eq();
void lt();
void le();
void gt();
void ge();
void ne();
void jmp(int x);
void brt(int x);
void brf(int x);
void call(int n);
void ret();
void pushr();
void popr();
void drop(int n);
void dup();

void  new7(int a);
void  getf(int a);
void  putf(int a);
void  newa();
void  getfa();
void  putfa();
void  getsz();
void  pushn();
void  refeq();
void  refne();

//global structure of objref
typedef struct {
unsigned int size;  /* byte count of payload data */
unsigned char data [1]; /* payload data , size as needed */
} * ObjRef ;

typedef struct {

 bool isObjRef;
 union {
       ObjRef objRef;
        int number;
 }u;
 
}StackSlot;
extern ObjRef *staticDataArea;
 extern StackSlot *stack;

void pusho(ObjRef objRef);
ObjRef popo();
ObjRef anlegen(int c);

void *createSpaceInHeap(int size);
void startGC();
void scanPhase();
ObjRef relocate(ObjRef orig);

ObjRef newCompoundObject(int etwas);


extern size_t heapAktuellSizeInBytes;
extern size_t stackAktuellSizeInBytes;

extern size_t heapAktuellSize;
extern size_t stackAktuellSize;
extern uint8_t *heap;
extern uint8_t *quellHalbSpeicher;
extern uint8_t *zielHalbSpeicher;



