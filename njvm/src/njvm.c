#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "njvm.h"
#include <stdbool.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0


#define HALT 0
#define PUSHC 1
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define MOD 6
#define RDINT 7
#define WRINT 8
#define RDCHAR 9
#define WRCHAR 10
#define PUSHG 11
#define POPG 12
#define ASF 13
#define RSF 14
#define PUSHL 15
#define POPL 16
#define EQ 17
#define NE 18
#define LT 19
#define LE 20
#define GT 21
#define GE 22
#define JMP 23
#define BRF 24
#define BRT 25
#define CALL 26
#define RET 27
#define DROP 28
#define PUSHR 29
#define POPR 30
#define DUP 31
#define NEW 32
#define GETF 33
#define PUTF 34
#define NEWA 35
#define GETFA 36
#define PUTFA 37
#define GETSZ 38
#define PUSHN 39
#define REFEQ 40
#define REFNE 41

#define IMMEDIATE(x) ((x)&0x00FFFFFF)
#define SIGN_EXTEND(i) ((i)&0x00800000 ? (i) | 0xFF000000 : (i))
#define STACK_SIZE 10000

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TODO

#define MSB (1 << (8 * sizeof(unsigned int) - 1))
#define IS_PRIM(objRef) (((objRef)->size & MSB) == 0)
#define GET_SIZE(objRef) ((objRef)->size & ~MSB)
#define GET_REFS(objRef) ((ObjRef *)(objRef)->data)

#define BROKEN_HEART ((1) << ((8 * sizeof(unsigned int)) - 2))
#define IS_BROKEN_HEART_PRESENT(obj) (((obj)->size & BROKEN_HEART) != 0)
#define FORWARD_POINTER_MASK 0x3FFFFFFF

#define COMPOBJECT_REAL_SIZE(obj) (sizeof(uint32_t) + (GET_SIZE(obj) * sizeof(ObjRef)))
#define PRIMOBJECT_REAL_SIZE(obj) (sizeof(uint32_t) + ((obj)->size * sizeof(uint8_t)))

#define stack_def_size 64
#define heap_def_size 8192



/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int halte = 0;

int sp = 0; // Stack Pointer
int fp = 0; // Frame Pointer
unsigned int IR;
unsigned int progCounter = 0; // programm Counter.
int c = 0;
// int staticDataArea[10000];

FILE *file = NULL;
char feld[4];
int version_number;
int num_instructions;
int num_variables;
int *static_data;
 int *instruction_Speicher;
int len = 0;

// UNTERPROGRAM AUFRUF.
int ra = 0;
///////
ObjRef rvr;
/////

//////////////////////////////////////////////////////////////
// TODO VARIABLES
int laufen = 1;
int gcpurge = 0;

int numOfVsda; // number of integers in static data area

size_t heapAktuellSizeInBytes;
size_t stackAktuellSizeInBytes;
size_t heapAktuellSize = heap_def_size;
size_t stackAktuellSize = stack_def_size;
uint8_t *heap;
uint8_t *quellHalbSpeicher;
uint8_t *zielHalbSpeicher;
uint8_t *heapFreiPointer;
uint8_t flip = 0;
uint32_t obectInHeap = 0;
uint32_t objectKopiertInGC = 0;
int gc_active = 0;

//////////////////////////////////////////////////////////////////////

// global structure of objref


StackSlot *stack;
ObjRef *staticDataArea;
 // ObjRef returnValue;

 const char *toOpen;


int main(int argc, char const *argv[])
{

    /* if (argc < 2)
    {
        printf("Error: File name not provided\n");
        printf("Use '--help' option for more information.\n");
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0)
    {
        help();
    }
    else if (strcmp(argv[1], "--version") == 0)
    {
        printf("NVM  4.0.0\n");
    } */

    printf("Ninja Virtual Machine started\n");
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--version") == 0)
        {
            printf("Ninja Virtual Machine version 0 (compiled Sep 23 2015, 10:36:52)\n");
            exit(0);
        }
        // ./njvm --help --version
        else if (strcmp(argv[i], "--help") == 0)
        {
            printf("usage: ./njvm [option] [option] ...\n");
            printf(" --version        show version and exit\n");
            printf(" --help           show this help and exit\n");
            exit(0);
        }
        else if (strcmp(argv[i], "--stack") == 0) {
            int ss = atoi(argv[i+1]);
            if (ss > 0) {
                stackAktuellSize = ss;
            }
        }
        else if (strcmp(argv[i], "--heap") == 0) {
            int ss = atoi(argv[i+1]);
            if (ss > 0) {
                heapAktuellSize = ss;
            }
        }
        else if (strcmp(argv[i], "--gcpurge") == 0)  
        {
            gcpurge = 1;
        }
        else if (argc > 8)
        {
            printf("unknown command line argument '%s',  try './njvm --help' \n", argv[i]);
            exit(1);
        } 
       else
    {
          int val = atoi(argv[i]);
            if ((val == 0) && (argv[i][0] != '0')) {
                toOpen = argv[i];
            } 
            
        
    }
    }
     executeNVM(toOpen);
    execute(instruction_Speicher);
    printf("Ninja Virtual Machine stopped\n");

    return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Execute //

void execute(int code[])
{

    /* while (!halte)
    {
        IR = code[progCounter];
        progCounter += 1;
        printer(IR);
    }

    progCounter = 0;
    halte = 0; */
    while (!halte)
    {
        IR = code[progCounter];
        progCounter += 1;
        ausfuehren(IR);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Print //

void printer(unsigned int IR)
{
    int printer = IR >> 24;
    switch (printer)
    {
    case PUSHC:
        printf("%03d\tpushc\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case ADD:
        printf("%03d\tadd\t\t\n", c);
        break;
    case SUB:
        printf("%03d\tsub\t\t\n", c);
        break;
    case MUL:
        printf("%03d\tmul\t\t\n", c);
        break;
    case DIV:
        printf("%03d\tdiv\t\t\n", c);
        break;
    case MOD:
        printf("%03d\tmod\t\t\n", c);
        break;
    case RDCHAR:
        printf("%03d\trdcahr\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case WRINT:
        printf("%03d\twrint\t\t\n", c);
        break;
    case RDINT:
        printf("%03d\trdint\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case WRCHAR:
        printf("%03d\twrchar\t\n", c); // Fehler
        break;
    case PUSHG:
        printf("%03d\tpushg\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case POPG:
        printf("%03d\tpopg\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case POPL:
        printf("%03d\tpopl\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case PUSHL:
        printf("%03d\tpushl\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case HALT:
        printf("%03d\thalt\t\t\n", c);
        halte = 1;
        break;
    case ASF:
        printf("%03d\tasf\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case RSF:
        printf("%03d\trsf\t\t\n", c);
        break;
    case EQ:
        printf("%03d\teq\t\t\n", c);
        break;
    case NE:
        printf("%03d\tne\t\t\n", c);
        break;
    case LT:
        printf("%03d\tlt\t\t\n", c);
        break;
    case LE:
        printf("%03d\tle\t\t\n", c);
        break;
    case GT:
        printf("%03d\tgt\t\t\n", c);
        break;
    case GE:
        printf("%03d\tge\t\t\n", c);
        break;
    case BRF:
        printf("%03d\tbrf\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case BRT:
        printf("%03d\tbrt\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case JMP:
        printf("%03d\tjmp\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case CALL:
        printf("%03d\tcall\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case RET:
        printf("%03d\tret\t\t\n", c);
        break;
    case DROP:
        printf("%03d\tdrop\t%d\t\n", c, SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case DUP:
        printf("%03d\tdup\t\t\n", c);
        break;
    case PUSHR:
        printf("%03d\tpushr\t\t\n", c);
        break;
    case POPR:
        printf("%03d\tpopr\t\t\n", c);
        break;
    }
    c++;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ausführen //

void ausfuehren(unsigned int IR)
{

    int op = IR >> 24;

    switch (op)
    {
    case PUSHC:
        bigFromInt(SIGN_EXTEND(IMMEDIATE(IR)));
        pusho(bip.res);
        break;
    case ADD:
        add();
        break;
    case SUB:
        sub();
        break;
    case MUL:
        mul();
        break;
    case DIV:
        divison();
        break;
    case MOD:
        mod();
        break;
    case RDINT:
        rdint();
        break;
    case WRINT:
        wrint();
        break;
    case RDCHAR:
        rdchar();
        break;
    case WRCHAR:
        wrchar();
        break;
    case HALT:
        halte = 1;
        break;
    case PUSHG:
        pushg(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case POPG:
        popg(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case ASF:
        asf(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case RSF:
        rsf();
        break;
    case PUSHL:
        pushl(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case POPL:
        popl(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case EQ:
        eq();
        break;
    case NE:
        ne();
        break;
    case LT:
        lt();
        break;
    case GT:
        gt();
        break;
    case LE:
        le();
        break;
    case GE:
        ge();
        break;
    case BRF:
        brf(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case BRT:
        brt(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case JMP:
        jmp(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case CALL:
        call(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case RET:
        ret();
        break;
    case DROP:
        drop(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case DUP:
        dup();
        break;
    case PUSHR:
        pushr();
        break;
    case POPR:
        popr();
        break;
    case NEW:
        new7(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case GETF:
        getf(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case PUTF:
        putf(SIGN_EXTEND(IMMEDIATE(IR)));
        break;
    case NEWA:
        newa();
        break;
    case GETFA:
        getfa();
        break;
    case PUTFA:
        putfa();
        break;
    case GETSZ:
        getsz();
        break;
    case PUSHN:
        pushn();
        break;
    case REFEQ:
        refeq();
        break;
    case REFNE:
        refne();
        break;
    default:
        printf("unkown opcode %d\n", op);
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stack //

void push(int a)
{
    if (sp > STACK_SIZE)
    {
        printf("Stack Overflow");
        return;
    }
    stack[sp].isObjRef = false;
    stack[sp].u.number = a;
    sp++;
}

int pop(void)
{

    int ele;
    // printf("pop %d\n",stack[sp-1]);
    sp--;
    ele = stack[sp].u.number;

    return ele;
}

void add()
{

    bip.op2 = popo();
    bip.op1 = popo();

    // int sum = pop1 + pop2;
    bigAdd();
    pusho(bip.res);
    // pusho(anlegen(sum));
}

void sub()
{

    bip.op2 = popo();
    bip.op1 = popo();

    // int sub = pop2 - pop1;
    bigSub();
    pusho(bip.res);

    // pusho(anlegen(sub));
}

void mul()
{

    bip.op2 = popo();
    bip.op1 = popo();
    bigMul();
    // int mul = pop1 * pop2;
    pusho(bip.res);

    //  pusho(anlegen(mul));
}

void divison()
{

    bip.op2 = popo();
    bip.op1 = popo();
    if (bip.op1 == 0)
    {
        printf("Sie können nicht durch Null dividieren\n");
        exit(1);
        // Frag mal, ob dieses Code richtig ist.
    }
    else
    {
        bigDiv();
        //// int div = pop1 / pop2;
        //  pusho(anlegen(div));
        pusho(bip.res);
    }
}

void mod()
{

    bip.op2 = popo();
    bip.op1 = popo();

    // int mod = pop1 % pop2;
    bigDiv();
    pusho(bip.rem);

    // pusho(anlegen(mod));
}

void rdint()
{
    // int x;
    // scanf("%d", &x);
    bigRead(stdin);
    pusho(bip.res);
    // pusho(anlegen(x));
}

void wrint()
{
    bip.op1 = popo();
    bigPrint(stdout);
    // printf("%d", x);
}

void rdchar()
{
    char x;
    scanf(" %c", &x);
    bigFromInt((int)x);
    pusho(bip.res);

    // pusho(anlegen(x));
}

void wrchar()
{
    int x;
    bip.op1 = popo();
    x = bigToInt();

    printf("%c", (char)x);
}

void halt()
{
    /* printf("%03d\thalt\t\t\n", c);
    wrint();*/
    halte = 1;
    printf("Ninja Virtual Machine stopped\n");
    // exit(0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// push global variable und pop global variable //

void pushg(int x)
{

    ObjRef y = staticDataArea[x];
    pusho(y);
}

void popg(int x)
{

    ObjRef y = popo();

    staticDataArea[x] = y;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// allocate stack12 frame und release stack12 frame //

void asf(int n)
{

    push(fp);
    fp = sp;
    sp = sp + n;
}

void rsf()
{
    sp = fp;
    fp = pop();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// push local variable und pop local variable //
void pushl(int n)
{
    pusho(stack[fp + n].u.objRef);
}

void popl(int n)
{

    stack[fp + n].u.objRef = popo();
    stack[fp + n].isObjRef = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KONTROLLSTRUKTUREN IN DER VM //

void eq()
{
    bip.op2 = popo();
    bip.op1 = popo();
    int result = bigCmp();
    if (result == 0)
    {
        bigFromInt(1);
        pusho(bip.res);
    }
    else
    {
        bigFromInt(0);
        pusho(bip.res);
    }
}
void lt()
{
    bip.op2 = popo();
    bip.op1 = popo();
    int result = bigCmp();
    if (result < 0)
    {
        bigFromInt(1);
        pusho(bip.res);
    }
    else
    {
        bigFromInt(0);
        pusho(bip.res);
    }
}
void le()
{
    bip.op2 = popo();
    bip.op1 = popo();
    int result = bigCmp();
    if (result <= 0)
    {
        bigFromInt(1);
        pusho(bip.res);
    }
    else
    {
        bigFromInt(0);
        pusho(bip.res);
    }
}

void gt()
{
    bip.op2 = popo();
    bip.op1 = popo();
    int result = bigCmp();
    if (result > 0)
    {
        bigFromInt(1);
        pusho(bip.res);
    }
    else
    {
        bigFromInt(0);
        pusho(bip.res);
    }
}
void ge()
{
    bip.op2 = popo();
    bip.op1 = popo();
    int result = bigCmp();
    if (result >= 0)
    {
        bigFromInt(1);
        pusho(bip.res);
    }
    else
    {
        bigFromInt(0);
        pusho(bip.res);
    }
}

void ne()
{
    bip.op2 = popo();
    bip.op1 = popo();
    int result = bigCmp();
    if (result != 0)
    {
        bigFromInt(1);
        pusho(bip.res);
    }
    else
    {
        bigFromInt(0);
        pusho(bip.res);
    }
}
void jmp(int n)
{
    progCounter = n;
}
void brt(int x)
{
    bip.op1 = popo();
    int tmp = bigToInt();
    if (tmp == 1)
        progCounter = x;
}
void brf(int x)
{
    bip.op1 = popo();
    int tmp = bigToInt();
    if (tmp == 0)
        progCounter = x;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UNTERPROGRAMM AUFRUF. //

void call(int n)
{
    ra = progCounter;
    push(ra);
    jmp(n);
}

void ret()
{
    ra = pop();
    jmp(ra);
}

void pushr()
{
    pusho(rvr);
}

void popr()
{
    rvr = popo();
}

void drop(int n)
{

    sp -= n;
    // if (n != 0)
    // {
    //     for (int i = 0; i < n; i++)
    //     {
    //         popo();
    //     }
    // }
}

void dup()
{
    ObjRef x = popo();
    pusho(x);
    pusho(x);
}
/////////////////////////////////////////////////////////////////////////////
/// Instructions von Aufgabe 8.

void new7(int n)
{
    ObjRef obj = newCompoundObject(n);
    pusho(obj);
}

void getf(int n)
{
    ObjRef obj1 = popo();
    ObjRef ob2 = GET_REFS(obj1)[n];
    pusho(ob2);
}

void putf(int n)
{
    ObjRef obj1 = popo(); 
    ObjRef obj2 = popo(); 
                          
    GET_REFS(obj2)[n] = obj1;
}

void newa()
{
    bip.op1 = popo();
    int arraySize = bigToInt();
    ObjRef obj = newCompoundObject(arraySize);
    pusho(obj);
}

void getfa()
{
    bip.op1 = popo(); 
    int ind = bigToInt();
    ObjRef obj = popo();
    pusho(GET_REFS(obj)[ind]);
}

void putfa()
{
    ObjRef obj1 = popo(); 
    bip.op1 = popo();    
    ObjRef obj2 = popo(); 
    int ind = bigToInt();
    GET_REFS(obj2)[ind] = obj1;
}

void getsz()
{
    ObjRef obj = popo();
    int objSize = GET_SIZE(obj);
    bigFromInt(objSize);
    pusho(bip.res);
}

void pushn()
{
    ObjRef obj = NULL;
    pusho(obj);
}

void refeq()
{
    ObjRef obj1 = popo();
    ObjRef obj2 = popo();
    if (obj1 == obj2)
    {
        bigFromInt(1);
        pusho(bip.res);
    }
    else
    {
        bigFromInt(0);
        pusho(bip.res);
    }
}

void refne()
{
    ObjRef obj1 = popo();
    ObjRef obj2 = popo();
    if (obj1 != obj2)
    {
        bigFromInt(1);
        pusho(bip.res);
    }
    else
    {
        bigFromInt(0);
        pusho(bip.res);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HELP //.

void help()
{
    printf("Usage: ./auf4 <Options | source file>\n ");
    printf("--version \t\t Show version number and quit\n");
    printf(" --help \t\t Show this help message and quit\n");
}

void executeNVM(const char *filename)
{


   // printf("Ninja Virtual Machine started\n");
    file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file\n");
        exit(1);
    }
    // step 1
    // identifies the format
     len = fread(&feld[0], sizeof(char), 4, file);
    /* if (memcmp(feld, "NJBF", 4) != 0)
    {
        printf("Error: Invalid file format\n");
        exit(1);
    } */

    if (len != 4)
    {
       printf("Error:File has format different from NJBF\n"); 
    }
    
    // step 2
    // Version Number
     len = fread(&version_number, sizeof(int), 1, file);
    if ( !(len == 1 && version_number <= 8))
    {
        printf("Error: Version number mismatch\n");
        exit(1);
    }

    // step 3
    // Read the number of instructions

    // TODO Wenn Fehler gäbe, hier checken..........
    // (unsigned int*)
    len =  fread(&num_instructions, sizeof(int), 1, file);
   // instruction_Speicher = malloc(num_instructions * sizeof(int));
     if (len != 1)
    {
        printf("Error:number of instructions is incorrect");
        exit(99);
    }



    // step 4
    // Read the number of variables in the static data area
     len = fread(&num_variables, sizeof(int), 1, file);
      if (len != 1)
    {
        printf("Error: number of instructions in static data area is not correct");
        exit(99);
    }
    staticDataArea = malloc(num_variables * sizeof(ObjRef) /*sizeof(int)*/);
    if (staticDataArea == NULL)
    {
       printf("Unable to locate Memory");
       exit(99);
    }
    

    // step 5
    // Read the rest of the file into the memory allocated in step 3).
        instruction_Speicher = ( int *) malloc(num_instructions * sizeof( int));

     len = fread(instruction_Speicher, sizeof(int), num_instructions, file);

    /////////////////////////////////////////////////

    if (fclose(file) != 0)
    {
            perror("Erorr (fclose)");
    }
    

    stackAktuellSizeInBytes = stackAktuellSize * 1024;
    heapAktuellSizeInBytes = heapAktuellSize * 1024;
    stack = malloc(stackAktuellSizeInBytes);
    heap = malloc(heapAktuellSizeInBytes);
    quellHalbSpeicher = heap;
    zielHalbSpeicher = heap + (heapAktuellSizeInBytes / 2);
    heapFreiPointer = heap;

    ///////////////////////////////////////////////7

   

   
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Aufgabe  5 RechenObjekte in Heap

void pusho(ObjRef objRef)
{

    stack[sp].isObjRef = true;
    stack[sp].u.objRef = objRef;
    sp++;
}

ObjRef popo(void)
{
    ObjRef ele;
    // printf("pop %d\n",stack[sp-1]);

    ele = stack[--sp].u.objRef;

    // printf("poped %d",*(int*)ele->data);

    return ele;
}

/*
ObjRef anlegen(int value)
{

  ObjRef objRef = malloc(sizeof(unsigned int) + sizeof(int));
  objRef->size = sizeof(int);
  *(int *)objRef->data = value;

  return objRef;
} */

void * newPrimObject(int dataSize)	/* create a new primitive object */
{
    ObjRef obj;
    size_t objSize = sizeof(unsigned int) + dataSize * sizeof(unsigned char);
    //obj = malloc(obj_size);
    obj = (ObjRef)createSpaceInHeap(objSize);
    if (obj== NULL) {
        fatalError("newPrimObject() got no memory");
    }
    obj->size = dataSize;
    return obj;
}

void * getPrimObjectDataPointer(void * primObject)
{
    return ((ObjRef)primObject)->data;
}


void fatalError(char *message)
{
    printf("Error :%s\n", message);
    exit(EXIT_FAILURE);
}

//////////////////////////////////////////////////////////////////////////////////////

void printGarbageCollectorInfos()
{
    printf("heapTotalSize in Bytes: %ld\n", heapAktuellSizeInBytes);
    printf("heap: %p\n", heap);
    printf("quellHalbSpeicher: %p\n", quellHalbSpeicher);
    printf("zielHalbSpeicher: %p\n", zielHalbSpeicher);
    printf("heapFreiPointer: %p\n", heapFreiPointer);
    printf("Object present in Heap %d \n", obectInHeap);
    printf("Object copied %d \n", objectKopiertInGC);
}

void *createSpaceInHeap(int sizeInBytes)
{
    void *newObj;
    uint8_t *aktuellZielHalbSpeicher = flip == 0 ? zielHalbSpeicher : quellHalbSpeicher;
    uint8_t *limit = quellHalbSpeicher == heap ? aktuellZielHalbSpeicher : (heap + heapAktuellSizeInBytes);
    if ((heapFreiPointer + sizeInBytes) > limit)
    {
        if (!gc_active)
            startGC();
    }
    limit = quellHalbSpeicher == heap ? zielHalbSpeicher : (heap + heapAktuellSizeInBytes);

    if ((heapFreiPointer + sizeInBytes) > limit)
    {
        fatalError("heap overflow");
    }

    newObj = heapFreiPointer;
    heapFreiPointer += sizeInBytes;
    obectInHeap += 1;

    return newObj;
}

void scanPhase()
{
    uint8_t *scan = quellHalbSpeicher;
    while (scan != heapFreiPointer)
    {
        ObjRef obj = (ObjRef)(scan);
        size_t objSize = COMPOBJECT_REAL_SIZE(obj);
        if (IS_PRIM(obj))
        {
            objSize = PRIMOBJECT_REAL_SIZE(obj);
        }
        
        if (!IS_PRIM(obj))
        {
            int length = GET_SIZE(obj);
            for (int i = 0; i < length; i++)
            {
                GET_REFS(obj)[i] = relocate(GET_REFS(obj)[i]);
            }
        }
        scan += objSize;
    }
}

ObjRef relocate(ObjRef orig)
{
    uint8_t *aktuellQuellHalbSpeicher = quellHalbSpeicher;

    ObjRef copy;
    if (orig == NULL)
    {
       
        copy = NULL;
    }
    else
    {
        if (IS_BROKEN_HEART_PRESENT(orig))
        {
            
            unsigned int forwardPointer = orig->size & FORWARD_POINTER_MASK;
            copy = (ObjRef)(aktuellQuellHalbSpeicher + forwardPointer);
        }
        else
        {
            size_t objSize = COMPOBJECT_REAL_SIZE(orig);
            if (IS_PRIM(orig))
            {
                
                objSize = PRIMOBJECT_REAL_SIZE(orig);
            }
            else
            {
                
            }
            copy = (ObjRef)createSpaceInHeap(objSize);
            memcpy(copy, orig, objSize);
            
            orig->size |= BROKEN_HEART; 
            orig->size &= 0xC0000000;  
            unsigned int forwardPointer = (unsigned int)((uint8_t *)copy - aktuellQuellHalbSpeicher);
            orig->size |= forwardPointer;
        }
    }
    objectKopiertInGC += 1;
   
    return copy;
}

void startGC()
{
    static int count = 0;
    count += 1;
    gc_active = 1;

    uint8_t *tmp = quellHalbSpeicher;
    quellHalbSpeicher = zielHalbSpeicher;
    zielHalbSpeicher = tmp;
    flip = (flip + 1) % 2;
    heapFreiPointer = quellHalbSpeicher;


    rvr = relocate(rvr);
    for (int i = 0; i < sp; i++)
    {
        if (stack[i].isObjRef)
        {
            stack[i].u.objRef = relocate(stack[i].u.objRef);
        }
    }

    bip.op1 = relocate(bip.op1);
    bip.op2 = relocate(bip.op2);
    bip.res = relocate(bip.res);
    bip.rem = relocate(bip.rem);

    for (int i = 0; i < num_variables; i++)
    {
        staticDataArea[i] = relocate(staticDataArea[i]);
    }
    scanPhase();
    obectInHeap = objectKopiertInGC;
    objectKopiertInGC = 0;
    gc_active = 0;
}

ObjRef newCompoundObject(int numObjRefs)
{
    ObjRef compObj;
    size_t objSize = sizeof(unsigned int) + (numObjRefs * sizeof(ObjRef));

    compObj = (ObjRef)createSpaceInHeap(objSize);
    if (compObj == NULL)
    {
        fatalError("newCompoundObject got no memory");
    }
    compObj->size = MSB | numObjRefs;
    for (int i = 0; i < numObjRefs; i++)
    {
        GET_REFS(compObj)[i] = NULL;
    }
    return compObj;
}