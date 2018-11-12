#include "hw.h"
#include "op.h"

const char *OpNames[] =
{
    "T0LVL",
    "T1LVL",
    "READ",
    "RDSEXPR",
    "RDLIST",
    "RDDOT",
    "RDQUOTE",
    "VALUEPRINT",
    "P0LIST",
    "P1LIST",
    "EVAL",
    "E0ARGS",
    "E1ARGS",
    "APPLY",
    "DOMACRO",
    "LAMBDA",
    "QUOTE",
    "DEF0",
    "DEF1",
    "BEGIN",
    "IF0",
    "IF1",
    "SET0",
    "SET1",
    "LET0",
    "LET1",
    "LET2",
    "LET0AST",
    "LET1AST",
    "LET2AST",
    "LET0REC",
    "LET1REC",
    "LET2REC",
    "COND0",
    "COND1",
    "DELAY",
    "AND0",
    "AND1",
    "OR0",
    "OR1",
    "C0STREAM",
    "C1STREAM",
    "0MACRO",
    "1MACRO",
    "CASE0",
    "CASE1",
    "CASE2",
    "PEVAL",
    "PAPPLY",
    "CONTINUATION",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "REM",
    "CAR",
    "CDR",
    "CONS",
    "SETCAR",
    "SETCDR",
    "NOT",
    "BOOL",
    "NULL",
    "ZEROP",
    "POSP",
    "NEGP",
    "NEQ",
    "LESS",
    "GRE",
    "LEQ",
    "GEQ",
    "SYMBOL",
    "NUMBER",
    "STRING",
    "PROC",
    "PAIR",
    "EQ",
    "EQV",
    "FORCE",
    "WRITE",
    "DISPLAY",
    "NEWLINE",
    "ERR0",
    "ERR1",
    "REVERSE",
    "APPEND",
    "QUIT",
    "GC",
    "GCVERB",
    "NEWSEGMENT",
    "LIST_LENGTH",
    "ASSQ",
    "GET_CLOSURE",
    "CLOSUREP",
    "MACROP",
}
;
ScmFunc dispatch_table[] =
{
    opexe_0,  // OP_T0LVL
    opexe_0,  // OP_T1LVL
    opexe_read,  // OP_READ
    opexe_read,  // OP_RDSEXPR
    opexe_read,  // OP_RDLIST
    opexe_read,  // OP_RDDOT
    opexe_read,  // OP_RDQUOTE
    opexe_print,  // OP_VALUEPRINT
    opexe_print,  // OP_P0LIST
    opexe_print,  // OP_P1LIST
    opexe_0,  // OP_EVAL
    opexe_0,  // OP_E0ARGS
    opexe_0,  // OP_E1ARGS
    opexe_0,  // OP_APPLY
    opexe_0,  // OP_DOMACRO
    opexe_0,  // OP_LAMBDA
    opexe_0,  // OP_QUOTE
    opexe_0,  // OP_DEF0
    opexe_0,  // OP_DEF1
    opexe_0,  // OP_BEGIN
    opexe_0,  // OP_IF0
    opexe_0,  // OP_IF1
    opexe_0,  // OP_SET0
    opexe_0,  // OP_SET1
    opexe_0,  // OP_LET0
    opexe_0,  // OP_LET1
    opexe_0,  // OP_LET2
    opexe_0,  // OP_LET0AST
    opexe_0,  // OP_LET1AST
    opexe_0,  // OP_LET2AST
    opexe_1,  // OP_LET0REC
    opexe_1,  // OP_LET1REC
    opexe_1,  // OP_LET2REC
    opexe_1,  // OP_COND0
    opexe_1,  // OP_COND1
    opexe_1,  // OP_DELAY
    opexe_1,  // OP_AND0
    opexe_1,  // OP_AND1
    opexe_1,  // OP_OR0
    opexe_1,  // OP_OR1
    opexe_1,  // OP_C0STREAM
    opexe_1,  // OP_C1STREAM
    opexe_1,  // OP_0MACRO
    opexe_1,  // OP_1MACRO
    opexe_1,  // OP_CASE0
    opexe_1,  // OP_CASE1
    opexe_1,  // OP_CASE2
    opexe_1,  // OP_PEVAL
    opexe_1,  // OP_PAPPLY
    opexe_1,  // OP_CONTINUATION
    opexe_2,  // OP_ADD
    opexe_2,  // OP_SUB
    opexe_2,  // OP_MUL
    opexe_2,  // OP_DIV
    opexe_2,  // OP_REM
    opexe_2,  // OP_CAR
    opexe_2,  // OP_CDR
    opexe_2,  // OP_CONS
    opexe_2,  // OP_SETCAR
    opexe_2,  // OP_SETCDR
    opexe_3,  // OP_NOT
    opexe_3,  // OP_BOOL
    opexe_3,  // OP_NULL
    opexe_3,  // OP_ZEROP
    opexe_3,  // OP_POSP
    opexe_3,  // OP_NEGP
    opexe_3,  // OP_NEQ
    opexe_3,  // OP_LESS
    opexe_3,  // OP_GRE
    opexe_3,  // OP_LEQ
    opexe_3,  // OP_GEQ
    opexe_3,  // OP_SYMBOL
    opexe_3,  // OP_NUMBER
    opexe_3,  // OP_STRING
    opexe_3,  // OP_PROC
    opexe_3,  // OP_PAIR
    opexe_3,  // OP_EQ
    opexe_3,  // OP_EQV
    opexe_4,  // OP_FORCE
    opexe_4,  // OP_WRITE
    opexe_4,  // OP_DISPLAY
    opexe_4,  // OP_NEWLINE
    opexe_4,  // OP_ERR0
    opexe_4,  // OP_ERR1
    opexe_4,  // OP_REVERSE
    opexe_4,  // OP_APPEND
    opexe_4,  // OP_QUIT
    opexe_4,  // OP_GC
    opexe_4,  // OP_GCVERB
    opexe_4,  // OP_NEWSEGMENT
    opexe_6,  // OP_LIST_LENGTH
    opexe_6,  // OP_ASSQ
    opexe_6,  // OP_GET_CLOSURE
    opexe_6,  // OP_CLOSUREP
    opexe_6,  // OP_MACROP
}
;
