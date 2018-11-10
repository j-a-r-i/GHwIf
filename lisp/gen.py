import sys

sys.path.append("../../CodeGen")


from codegen import OClass, OEnum, OMethod, OStruct, OMacro, OArg, OSwitch, OArrStr
from codegen import write_file_n, q



OPS = [
[ "OP_LOAD", "load", 0, "opexe_0", "proc" ],
 [ "OP_T0LVL", None, 1, "opexe_0", None ],
 [ "OP_T1LVL", None, 2, "opexe_0", None ],
 [ "OP_READ", "read", 3, "opexe_0", "proc" ],
 [ "OP_VALUEPRINT", None, 4, "opexe_0", None ],
 [ "OP_EVAL", None, 5, "opexe_0", None ],
 [ "OP_E0ARGS", None, 6, "opexe_0", None ],
 [ "OP_E1ARGS", None, 7, "opexe_0", None ],
 [ "OP_APPLY", None, 8, "opexe_0", None ],
 [ "OP_DOMACRO", None, 9, "opexe_0", None ],
 [ "OP_LAMBDA", "lambda", 10, "opexe_0", "syntax" ],
 [ "OP_QUOTE", "quote", 11, "opexe_0", "syntax" ],
 [ "OP_DEF0", "define", 12, "opexe_0", "syntax" ],
 [ "OP_DEF1", None, 13, "opexe_0", None ],
 [ "OP_BEGIN", "begin", 14, "opexe_0", "syntax" ],
 [ "OP_IF0", "if", 15, "opexe_0", "syntax" ],
 [ "OP_IF1", None, 16, "opexe_0", None ],
 [ "OP_SET0", "set!", 17, "opexe_0", "syntax" ],
 [ "OP_SET1", None, 18, "opexe_0", None ],
 [ "OP_LET0", "let", 19, "opexe_0", "syntax" ],
 [ "OP_LET1", None, 20, "opexe_0", None ],
 [ "OP_LET2", None, 21, "opexe_0", None ],
 [ "OP_LET0AST", "let*", 22, "opexe_0", "syntax" ],
 [ "OP_LET1AST", None, 23, "opexe_0", None ],
 [ "OP_LET2AST", None, 24, "opexe_0", None ],
 [ "OP_LET0REC", "letrec", 25, "opexe_1", "syntax" ],
 [ "OP_LET1REC", None, 26, "opexe_1", None ],
 [ "OP_LET2REC", None, 27, "opexe_1", None ],
 [ "OP_COND0", "cond", 28, "opexe_1", "syntax" ],
 [ "OP_COND1", None, 29, "opexe_1", None ],
 [ "OP_DELAY", "delay", 30, "opexe_1", "syntax" ],
 [ "OP_AND0", "and", 31, "opexe_1", "syntax" ],
 [ "OP_AND1", None, 32, "opexe_1", None ],
 [ "OP_OR0", "or", 33, "opexe_1", "syntax" ],
 [ "OP_OR1", None, 34, "opexe_1", None ],
 [ "OP_C0STREAM", "cons-stream", 35, "opexe_1", "syntax" ],
 [ "OP_C1STREAM", None, 36, "opexe_1", None ],
 [ "OP_0MACRO", "macro", 37, "opexe_1", "syntax" ],
 [ "OP_1MACRO", None, 38, "opexe_1", None ],
 [ "OP_CASE0", "case", 39, "opexe_1", "syntax" ],
 [ "OP_CASE1", None, 40, "opexe_1", None ],
 [ "OP_CASE2", None, 41, "opexe_1", None ],
 [ "OP_PEVAL", "eval", 42, "opexe_1", "proc" ],
 [ "OP_PAPPLY", "apply", 43, "opexe_1", "proc" ],
 [ "OP_CONTINUATION", "call-with-current-continuation", 44, "opexe_1", "proc" ],
 [ "OP_ADD", "+", 45, "opexe_2", "proc" ],
 [ "OP_SUB", "-", 46, "opexe_2", "proc" ],
 [ "OP_MUL", "*", 47, "opexe_2", "proc" ],
 [ "OP_DIV", "/", 48, "opexe_2", "proc" ],
 [ "OP_REM", "remainder", 49, "opexe_2", "proc" ],
 [ "OP_CAR", "car", 50, "opexe_2", "proc" ],
 [ "OP_CDR", "cdr", 51, "opexe_2", "proc" ],
 [ "OP_CONS", "cons", 52, "opexe_2", "proc" ],
 [ "OP_SETCAR", "set-car!", 53, "opexe_2", "proc" ],
 [ "OP_SETCDR", "set-cdr!", 54, "opexe_2", "proc" ],
 [ "OP_NOT", "not", 55, "opexe_3", "proc" ],
 [ "OP_BOOL", "boolean?", 56, "opexe_3", "proc" ],
 [ "OP_NULL", "null?", 57, "opexe_3", "proc" ],
 [ "OP_ZEROP", "zero?", 58, "opexe_3", "proc" ],
 [ "OP_POSP", "positive?", 59, "opexe_3", "proc" ],
 [ "OP_NEGP", "negative?", 60, "opexe_3", "proc" ],
 [ "OP_NEQ", "=", 61, "opexe_3", "proc" ],
 [ "OP_LESS", "<", 62, "opexe_3", "proc" ],
 [ "OP_GRE", ">", 63, "opexe_3", "proc" ],
 [ "OP_LEQ", "<=", 64, "opexe_3", "proc" ],
 [ "OP_GEQ", ">=", 65, "opexe_3", "proc" ],
 [ "OP_SYMBOL", "symbol?", 66, "opexe_3", "proc" ],
 [ "OP_NUMBER", "number?", 67, "opexe_3", "proc" ],
 [ "OP_STRING", "string?", 68, "opexe_3", "proc" ],
 [ "OP_PROC", "procedure?", 69, "opexe_3", "proc" ],
 [ "OP_PAIR", "pair?", 70, "opexe_3", "proc" ],
 [ "OP_EQ", "eq?", 71, "opexe_3", "proc" ],
 [ "OP_EQV", "eqv?", 72, "opexe_3", "proc" ],
 [ "OP_FORCE", "force", 73, "opexe_4", "proc" ],
 [ "OP_WRITE", "write", 74, "opexe_4", "proc" ],
 [ "OP_DISPLAY", "display", 75, "opexe_4", "proc" ],
 [ "OP_NEWLINE", "newline", 76, "opexe_4", "proc" ],
 [ "OP_ERR0", "error", 77, "opexe_4", "proc" ],
 [ "OP_ERR1", None, 78, "opexe_4", None ],
 [ "OP_REVERSE", "reverse", 79, "opexe_4", "proc" ],
 [ "OP_APPEND", "append", 80, "opexe_4", "proc" ],
 [ "OP_PUT", "put", 81, "opexe_4", "proc" ],
 [ "OP_GET", "get", 82, "opexe_4", "proc" ],
 [ "OP_QUIT", "quit", 83, "opexe_4", "proc" ],
 [ "OP_GC", "gc", 84, "opexe_4", "proc" ],
 [ "OP_GCVERB", "gc-verbose", 85, "opexe_4", "proc" ],
 [ "OP_NEWSEGMENT", "new-segment", 86, "opexe_4", "proc" ],
 [ "OP_RDSEXPR", None, 87, "opexe_5", None ],
 [ "OP_RDLIST", None, 88, "opexe_5", None ],
 [ "OP_RDDOT", None, 89, "opexe_5", None ],
 [ "OP_RDQUOTE", None, 90, "opexe_5", None ],
 [ "OP_RDQQUOTE", None, 91, "opexe_5", None ],
 [ "OP_RDUNQUOTE", None, 92, "opexe_5", None ],
 [ "OP_RDUQTSP", None, 93, "opexe_5", None ],
 [ "OP_P0LIST", None, 94, "opexe_5", None ],
 [ "OP_P1LIST", None, 95, "opexe_5", None ],
 [ "OP_LIST_LENGTH", "list-length", 96, "opexe_6", "proc" ],
 [ "OP_ASSQ", "assq", 97, "opexe_6", "proc" ],
 [ "OP_GET_CLOSURE", "get-closure-code", 98, "opexe_6", "proc" ],
 [ "OP_CLOSUREP", "closure?", 99, "opexe_6", "proc" ],
 [ "OP_MACROP", "macro?", 100, "opexe_6", "proc" ]
]


en = OEnum("OpCodes")
sl = OArrStr("const char *OpNames")
sf = OArrStr("ScmFunc dispatch_table")

for op in OPS:
    ident = op[0]
    func  = op[3]

    en.add(ident)
    sl.add(q(ident[3:]))
    sf.add([func, ident])

write_file_n("op", en, sl, sf)
