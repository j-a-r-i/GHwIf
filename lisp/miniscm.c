/*
 *      ---------- Mini-Scheme Interpreter Version 0.85 ----------
 *
 *                coded by Atsushi Moriwaki (11/5/1989)
 *
 *            E-MAIL :  moriwaki@kurims.kurims.kyoto-u.ac.jp
 *
 *               THIS SOFTWARE IS IN THE PUBLIC DOMAIN
 *               ------------------------------------
 * This software is completely free to copy, modify and/or re-distribute.
 * But I would appreciate it if you left my name on the code as the author.
 *
 */
/*--
 *
 *  This version has been modified by R.C. Secrist.
 *
 *  Mini-Scheme is now maintained by Akira KIDA.
 *
 *  This is a revised and modified version by Akira KIDA.
 *	current version is 0.85k4 (15 May 1994)
 *
 *  Please send suggestions, bug reports and/or requests to:
 *		<SDI00379@niftyserve.or.jp>
 *--
 */ 

#define OWN_REPL
/* #define VERBOSE */	/* define this if you want verbose GC */
#define	AVOID_HACK_LOOP	/* define this if your compiler is poor
			 * enougth to complain "do { } while (0)"
			 * construction.
			 */
#define USE_MACRO	/* undef this if you do not need macro */

/*
 *  Basic memory allocation units
 */

#define CELL_SEGSIZE    5000	/* # of cells in one segment */
#define CELL_NSEGMENT   100	/* # of segments for cells */
#define STR_SEGSIZE     2500	/* bytes of one string segment */
#define STR_NSEGMENT    100	/* # of segments for strings */


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include "miniscm.h"
#include "op.h"

#define prompt "> "
#define FIRST_CELLSEGS 10

#ifdef __GNUC__
/*
 * If we use gcc, AVOID_HACK_LOOP is unnecessary
 */
#undef AVOID_HACK_LOOP
#endif

#define T_STRING         1	/* 0000000000000001 */
#define T_NUMBER         2	/* 0000000000000010 */
#define T_SYMBOL         4	/* 0000000000000100 */
#define T_SYNTAX         8	/* 0000000000001000 */
#define T_PROC          16	/* 0000000000010000 */
#define T_PAIR          32	/* 0000000000100000 */
#define T_CLOSURE       64	/* 0000000001000000 */
#define T_CONTINUATION 128	/* 0000000010000000 */
#ifdef USE_MACRO
# define T_MACRO        256	/* 0000000100000000 */
#endif
#define T_PROMISE      512	/* 0000001000000000 */
#define T_FOREIGN     1024      
#define T_ATOM       16384	/* 0100000000000000 */	/* only for gc */
#define CLRATOM      49151	/* 1011111111111111 */	/* only for gc */
#define MARK         32768	/* 1000000000000000 */
#define UNMARK       32767	/* 0111111111111111 */

extern void FatalError(char *fmt);
extern void Error(char *fmt);
extern void init_globals();
extern void gc(cell *a, cell *b);

/* macros for cell operations */
#define type(p)         ((p)->flag)

#define isstring(p)     ((p)->flag & T_STRING)
#define strvalue(p)     ((p)->obj._string._svalue)
#define keynum(p)       ((p)->obj._string._keynum)

#define isnumber(p)     ((p)->flag & T_NUMBER)
#define ivalue(p)       ((p)->obj._ivalue)

#define ispair(p)       ((p)->flag & T_PAIR)
#define car(p)          ((p)->obj._cons._car)
#define cdr(p)          ((p)->obj._cons._cdr)

#define issymbol(p)     ((p)->flag & T_SYMBOL)
#define symname(p)      strvalue(car(p))
#define hasprop(p)      ((p)->flag & T_SYMBOL)
#define symprop(p)      cdr(p)

#define issyntax(p)     ((p)->flag & T_SYNTAX)
#define isproc(p)       ((p)->flag & T_PROC)
#define syntaxnum(p)    keynum(car(p))
#define procnum(p)      ivalue(p)

#define isclosure(p)    ((p)->flag & T_CLOSURE)
#ifdef USE_MACRO
# define ismacro(p)      ((p)->flag & T_MACRO)
#endif
#define closure_code(p) car(p)
#define closure_env(p)  cdr(p)

#define iscontinuation(p) ((p)->flag & T_CONTINUATION)
#define cont_dump(p)    cdr(p)

#define isatom(p)       ((p)->flag &T_ATOM)
#define clratom(p)      (p)->flag &= CLRATOM

#define ismark(p)       ((p)->flag & MARK)
#define clrmark(p)      (p)->flag &= UNMARK

#define caar(p)         car(car(p))
#define cadr(p)         car(cdr(p))
#define cdar(p)         cdr(car(p))
#define cddr(p)         cdr(cdr(p))
#define cadar(p)        car(cdr(car(p)))
#define caddr(p)        car(cdr(cdr(p)))
#define cadaar(p)       car(cdr(car(car(p))))
#define cadddr(p)       car(cdr(cdr(cdr(p))))
#define cddddr(p)       cdr(cdr(cdr(cdr(p))))

scm gScheme;

/* arrays for segments */
cell *cell_seg[CELL_NSEGMENT];
int     last_cell_seg = -1;
char   *str_seg[STR_NSEGMENT];
int     str_seglast = -1;

/* We use 4 registers. */
cell *args;			/* register for arguments of function */
cell *envir;			/* stack register for current environment */
cell *code;			/* register for current code */
cell *dump;			/* stack register for next evaluation */

struct cell _NIL;
cell *NIL = &_NIL;		/* special cell representing empty cell */
struct cell _T;
cell *T = &_T;		/* special cell representing #t */
struct cell _F;
cell *F = &_F;		/* special cell representing #f */
cell *oblist = &_NIL;		/* pointer to symbol table */
cell *global_env;		/* pointer to global environment */

/* global pointers to special symbols */
cell *LAMBDA;			/* pointer to syntax lambda */
cell *QUOTE;			/* pointer to syntax quote */

cell *free_cell = &_NIL;	/* pointer to top of free cells */
long    fcells = 0;		/* # of free cells */
FILE   *infp;			/* input file */
char    gc_verbose;		/* if gc_verbose is not zero, print gc status */

/* allocate new cell segment */
int alloc_cellseg(int n)
{
	register cell *p;
	register long i;
	register int k;

	for (k = 0; k < n; k++) {
		if (last_cell_seg >= CELL_NSEGMENT - 1)
			return k;
		p = (cell*) malloc(CELL_SEGSIZE * sizeof(cell));
		if (p == (cell*) 0)
			return k;
		cell_seg[++last_cell_seg] = p;
		fcells += CELL_SEGSIZE;
		for (i = 0; i < CELL_SEGSIZE - 1; i++, p++) {
			p->flag = 0;
			car(p) = NIL;
			cdr(p) = p + 1;
		}
		p->flag = 0;
		car(p) = NIL;
		cdr(p) = free_cell;
		free_cell = cell_seg[last_cell_seg];
	}
	return n;
}

/* allocate new string segment */
int alloc_strseg(int n)
{
	register char *p;
	register long i;
	register int k;

	for (k = 0; k < n; k++) {
		if (str_seglast >= STR_NSEGMENT)
			return k;
		p = (char *) malloc(STR_SEGSIZE * sizeof(char));
		if (p == (char *) 0)
			return k;
		str_seg[++str_seglast] = p;
		for (i = 0; i < STR_SEGSIZE; i++)
			*p++ = (char) (-1);
	}
	return n;
}

/* initialization of Mini-Scheme */
void init_scheme()
{
	register cell *i;

	if (alloc_cellseg(FIRST_CELLSEGS) != FIRST_CELLSEGS)
		FatalError("Unable to allocate initial cell segments");
	if (!alloc_strseg(1))
		FatalError("Unable to allocate initial string segments");
#ifdef VERBOSE
	gc_verbose = 1;
#else
	gc_verbose = 0;
#endif
	init_globals();
}

/* get new cell.  parameter a, b is marked by gc. */
cell *get_cell(cell *a, cell *b)
{
	register cell *x;

	if (free_cell == NIL) {
		gc(a, b);
		if (free_cell == NIL)
			if (!alloc_cellseg(1))
				FatalError("run out of cells  --- unable to recover cells");
	}
	x = free_cell;
	free_cell = cdr(x);
	--fcells;
	return (x);
}

/* get new cons cell */
cell *cons(cell *a, cell *b)
{
	register cell *x = get_cell(a, b);

	x->flag = T_PAIR;
	car(x) = a;
	cdr(x) = b;
	return (x);
}

/* get number atom */
cell *mk_number(long num)
{
	register cell *x = get_cell(NIL, NIL);

	x->flag = (T_NUMBER | T_ATOM);
	ivalue(x) = num;
	return (x);
}

/* allocate name to string area */
char   *store_string(const char *name)
{
	register char *q;
	register short i;
	long    len, remain;

	/* first check name has already listed */
	for (i = 0; i <= str_seglast; i++)
		for (q = str_seg[i]; *q != (char) (-1); ) {
			if (!strcmp(q, name))
				goto FOUND;
			while (*q++)
				;	/* get next string */
		}
	len = strlen(name) + 2;
	remain = (long) STR_SEGSIZE - ((long) q - (long) str_seg[str_seglast]);
	if (remain < len) {
		if (!alloc_strseg(1))
			FatalError("run out of string area");
		q = str_seg[str_seglast];
	}
	strcpy(q, name);
FOUND:
	return (q);
}

/* get new string */
cell *mk_string(const char *str)
{
	register cell *x = get_cell(NIL, NIL);

	strvalue(x) = store_string(str);
	x->flag = (T_STRING | T_ATOM);
	keynum(x) = (short) (-1);
	return (x);
}

/* get new symbol */
cell *mk_symbol(const char *name)
{
	register cell *x;

	/* fisrt check oblist */
	for (x = oblist; x != NIL; x = cdr(x))
		if (!strcmp(name, symname(car(x))))
			break;

	if (x != NIL)
		return (car(x));
	else {
		x = cons(mk_string(name), NIL);
		x->flag = T_SYMBOL;
		oblist = cons(x, oblist);
		return (x);
	}
}

/* make symbol or number atom from string */
cell *mk_atom(char *q)
{
	char    c, *p;

	p = q;
	if (!isdigit(c = *p++)) {
		if ((c != '+' && c != '-') || !isdigit(*p))
			return (mk_symbol(q));
	}
	for ( ; (c = *p) != 0; ++p)
		if (!isdigit(c))
			return (mk_symbol(q));
	return (mk_number(atol(q)));
}

/* make constant */
cell *mk_const(char *name)
{
	long    x;
	char    tmp[256];

	if (!strcmp(name, "t"))
		return (T);
	else if (!strcmp(name, "f"))
		return (F);
	else if (*name == 'o') {/* #o (octal) */
		sprintf(tmp, "0%s", &name[1]);
		sscanf(tmp, "%lo", &x);
		return (mk_number(x));
	} else if (*name == 'd') {	/* #d (decimal) */
		sscanf(&name[1], "%ld", &x);
		return (mk_number(x));
	} else if (*name == 'x') {	/* #x (hex) */
		sprintf(tmp, "0x%s", &name[1]);
		sscanf(tmp, "%lx", &x);
		return (mk_number(x));
	} else
		return (NIL);
}


/* ========== garbage collector ========== */

/*--
 *  We use algorithm E (Kunuth, The Art of Computer Programming Vol.1,
 *  sec.3.5) for marking.
 */
void mark(cell *a)
{
	register cell *t, *q, *p;

E1:	t = (cell*) 0;
	p = a;
E2:	p->flag |= MARK;
E3:	if (isatom(p))
		goto E6;
E4:	q = car(p);
	if (q && !ismark(q)) {
		p->flag |= T_ATOM;
		car(p) = t;
		t = p;
		p = q;
		goto E2;
	}
E5:	q = cdr(p);
	if (q && !ismark(q)) {
		cdr(p) = t;
		t = p;
		p = q;
		goto E2;
	}
E6:	if (!t)
		return;
	q = t;
	if (isatom(q)) {
		clratom(q);
		t = car(q);
		car(q) = p;
		p = q;
		goto E5;
	} else {
		t = cdr(q);
		cdr(q) = p;
		p = q;
		goto E6;
	}
}


/* garbage collection. parameter a, b is marked. */
void gc(cell *a, cell *b)
{
	register cell *p;
	register short i;
	register long j;

	if (gc_verbose)
		printf("gc...");

	/* mark system globals */
	mark(oblist);
	mark(global_env);

	/* mark current registers */
	mark(args);
	mark(envir);
	mark(code);
	mark(dump);

	/* mark variables a, b */
	mark(a);
	mark(b);

	/* garbage collect */
	clrmark(NIL);
	fcells = 0;
	free_cell = NIL;
	for (i = 0; i <= last_cell_seg; i++) {
		for (j = 0, p = cell_seg[i]; j < CELL_SEGSIZE; j++, p++) {
			if (ismark(p))
				clrmark(p);
			else {
				p->flag = 0;
				cdr(p) = free_cell;
				car(p) = NIL;
				free_cell = p;
				++fcells;
			}
		}
	}

	if (gc_verbose)
		printf(" done %ld cells are recovered.\n", fcells);
}


/* ========== Rootines for Reading ========== */

typedef enum {
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_DOT,
    TOK_ATOM,
    TOK_QUOTE,
    TOK_COMMENT,
    TOK_DQUOTE,
    TOK_SHARP
} EToken;    

const char *TokenNames[] = {
    "LPAREN",
    "RPAREN",
    "DOT",
    "ATOM",
    "QUOTE",
    "COMMENT",
    "DQUOTE",
    "SHARP"
};


#define LINESIZE 1024
char    linebuff[LINESIZE];
char    strbuff[256];
char   *currentline = linebuff;
char   *endline = linebuff;

/* get new character from input file */
int     inchar()
{
	if (currentline >= endline) {	/* input buffer is empty */
		if (feof(infp)) {
			fclose(infp);
			infp = stdin;
			printf(prompt);
		}
		strcpy(linebuff, "\n");
		if (fgets(currentline = linebuff, LINESIZE, infp) == NULL)
			if (infp == stdin) {
				fprintf(stderr, "Good-bye\n");
				exit(0);
			}
		endline = linebuff + strlen(linebuff);
	}
	return (*currentline++);
}

/* back to standard input */
void flushinput()
{
	if (infp != stdin) {
		fclose(infp);
		infp = stdin;
	}
	currentline = endline = linebuff;
}

/* back character to input buffer */
void backchar()
{
	currentline--;
}

/* check c is delimiter */
int isdelim(char *s, char c)
{
	while (*s)
		if (*s++ == c)
			return (0);
	return (1);
}

/* read chacters to delimiter */
char   *readstr(char *delim)
{
	char   *p = strbuff;

	while (isdelim(delim, (*p++ = inchar())))
		;
	backchar();
	*--p = '\0';
	return (strbuff);
}

/* read string expression "xxx...xxx" */
char   *readstrexp()
{
	char    c, *p = strbuff;

	for (;;) {
		if ((c = inchar()) != '"')
			*p++ = c;
		else if (p > strbuff && *(p - 1) == '\\')
			*(p - 1) = '"';
		else {
			*p = '\0';
			return (strbuff);
		}
	}
}


/* skip white characters */
void skipspace()
{
	while (isspace(inchar()))
		;
	backchar();
}

/* get token */
EToken token()
{
	skipspace();
	switch (inchar()) {
	case '(':
		return (TOK_LPAREN);
	case ')':
		return (TOK_RPAREN);
	case '.':
		return (TOK_DOT);
	case '\'':
		return (TOK_QUOTE);
	case ';':
		return (TOK_COMMENT);
	case '"':
		return (TOK_DQUOTE);
	case '#':
		return (TOK_SHARP);
	default:
		backchar();
		return (TOK_ATOM);
	}
}

/* ========== Rootines for Printing ========== */
#define	ok_abbrev(x)	(ispair(x) && cdr(x) == NIL)

void strunquote(char *p, char *s)
{
	*p++ = '"';
	for ( ; *s; ++s) {
		if (*s == '"') {
			*p++ = '\\';
			*p++ = '"';
		} else if (*s == '\n') {
			*p++ = '\\';
			*p++ = 'n';
		} else
			*p++ = *s;
	}
	*p++ = '"';
	*p = '\0';
}


/* print atoms */
void printatom(cell *l, int f)
{
	char	*p;
	
	if (l == NIL)
		p = "()";
	else if (l == T)
		p = "#t";
	else if (l == F)
		p = "#f";
	else if (isnumber(l)) {
		p = strbuff;
		sprintf(p, "%ld", ivalue(l));
	} else if (isstring(l)) {
		if (!f)
			p = strvalue(l);
		else {
			p = strbuff;
			strunquote(p, strvalue(l));
		}
	} else if (issymbol(l))
		p = symname(l);
	else if (isproc(l)) {
		p = strbuff;
		sprintf(p, "#<PROCEDURE %ld>", procnum(l));
#ifdef USE_MACRO
	} else if (ismacro(l)) {
		p = "#<MACRO>";
#endif
	} else if (isclosure(l))
		p = "#<CLOSURE>";
	else if (iscontinuation(l))
		p = "#<CONTINUATION>";
	gScheme.print(p);
}


/* ========== Rootines for Evaluation Cycle ========== */

/* make closure. c is code. e is environment */
cell *mk_closure(cell *c, cell *e)
{
	register cell *x = get_cell(c, e);

	x->flag = T_CLOSURE;
	car(x) = c;
	cdr(x) = e;
	return (x);
}

/* make continuation. */
cell *mk_continuation(cell *d)
{
	register cell *x = get_cell(NIL, d);

	x->flag = T_CONTINUATION;
	cont_dump(x) = d;
	return (x);
}

/* reverse list -- make new cells */
cell *reverse(cell *a)
		/* a must be checked by gc */
{
	register cell *p = NIL;

	for ( ; ispair(a); a = cdr(a))
		p = cons(car(a), p);
	return (p);
}

/* reverse list --- no make new cells */
cell *non_alloc_rev(cell *term, cell *list)
{
	register cell *p = list, *result = term, *q;

	while (p != NIL) {
		q = cdr(p);
		cdr(p) = result;
		result = p;
		p = q;
	}
	return (result);
}

/* append list -- make new cells */
cell *append(cell *a, cell *b)
{
	register cell *p = b, *q;

	if (a != NIL) {
		a = reverse(a);
		while (a != NIL) {
			q = cdr(a);
			cdr(a) = p;
			p = a;
			a = q;
		}
	}
	return (p);
}

/* equivalence of atoms */
int eqv(cell *a, cell *b)
{
	if (isstring(a)) {
		if (isstring(b))
			return (strvalue(a) == strvalue(b));
		else
			return (0);
	} else if (isnumber(a)) {
		if (isnumber(b))
			return (ivalue(a) == ivalue(b));
		else
			return (0);
	} else
		return (a == b);
}

/* true or false value macro */
#define istrue(p)       ((p) != NIL && (p) != F)
#define isfalse(p)      ((p) == NIL || (p) == F)

/* Error macro */
#ifdef	AVOID_HACK_LOOP
# define	BEGIN	{
# define	END	}
#else
/*
 * I believe this is better, but some compiler complains....
 */
# define	BEGIN	do {
# define	END	} while (0)
#endif

#define Error_0(s) BEGIN                       \
    args = cons(mk_string((s)), NIL);          \
    operator = (uint8_t)OP_ERR0;                 \
    return T; END

#define Error_1(s, a) BEGIN                    \
    args = cons((a), NIL);                     \
    args = cons(mk_string((s)), args);         \
    operator = (uint8_t)OP_ERR0;                 \
    return T; END

/* control macros for Eval_Cycle */
#define s_goto(a) BEGIN                        \
    operator = (short)(a);                     \
    return T; END

static cell *value;
static OpCodes operator;
static EToken tok;
static int print_flag;
static int par_level;

void s_save(OpCodes a, cell *b, cell *c)
{
    printf("<sav>");
    fflush(stdout);
    
    dump = cons(envir, cons((c), dump));
    dump = cons((b), dump);
    dump = cons(mk_number((long)(a)), dump);
}

cell *s_return(cell *a)
{
    printf("<ret>");
    fflush(stdout);
    
    value = (a);
    operator = ivalue(car(dump));
    args = cadr(dump);
    envir = caddr(dump);
    code = cadddr(dump);
    dump = cddddr(dump);
    return T;
}

#define s_retbool(tf)	return s_return((tf) ? T : F)



/* ========== Evaluation Cycle ========== */


cell *opexe_read(uint8_t op)
{
    register cell *x;
    
	switch (op) {
	case OP_READ:		/* read */
		tok = token();
		par_level = 0;
		s_goto(OP_RDSEXPR);

	case OP_RDSEXPR:
	    printf("{%s}", TokenNames[tok]);
		switch (tok) {
		case TOK_COMMENT:
			while (inchar() != '\n')
				;
			tok = token();
			s_goto(OP_RDSEXPR);
		case TOK_LPAREN:
		        par_level++;
			tok = token();
			if (tok == TOK_RPAREN) {
				return s_return(NIL);
			} else if (tok == TOK_DOT) {
				Error_0("syntax error -- illegal dot expression");
			} else {
				s_save(OP_RDLIST, NIL, NIL);
				s_goto(OP_RDSEXPR);
			}
		case TOK_QUOTE:
			s_save(OP_RDQUOTE, NIL, NIL);
			tok = token();
			s_goto(OP_RDSEXPR);
		case TOK_ATOM:
			return s_return(mk_atom(readstr("();\t\n ")));
		case TOK_DQUOTE:
			return s_return(mk_string(readstrexp()));
		case TOK_SHARP:
			if ((x = mk_const(readstr("();\t\n "))) == NIL) {
				Error_0("Undefined sharp expression");
			} else {
				return s_return(x);
			}
		default:
			Error_0("syntax error -- illegal token");
		}
		break;

	case OP_RDLIST:
		args = cons(value, args);
		tok = token();
	    printf("/%s/", TokenNames[tok]);
		if (tok == TOK_COMMENT) {
			while (inchar() != '\n')
				;
			tok = token();
		}
		if (tok == TOK_RPAREN) {
		        par_level--;
#ifdef OWN_REPL
			if (par_level == 0) {
			    value = non_alloc_rev(NIL, args);
			    return NIL;
			}
			else
#endif
			{
			    printf("(level=%d)", par_level);
			    return s_return(non_alloc_rev(NIL, args));
			}
		} else if (tok == TOK_DOT) {
			s_save(OP_RDDOT, args, NIL);
			tok = token();
			s_goto(OP_RDSEXPR);
		} else {
			s_save(OP_RDLIST, args, NIL);;
			s_goto(OP_RDSEXPR);
		}

	case OP_RDDOT:
		if (token() != TOK_RPAREN) {
			Error_0("syntax error -- illegal dot expression");
		} else {
			return s_return(non_alloc_rev(value, args));
		}

	case OP_RDQUOTE:
		return s_return(cons(QUOTE, cons(value, NIL)));

	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;
}


void scm_print(cell *arg)
{
    print_flag = 1;

    printf("_%X_", arg->flag);
    
    if (arg->flag != T_PAIR) {
	printatom(arg, print_flag);
	return;
    }
    else if (car(args) == QUOTE && ok_abbrev(cdr(args))) {
	gScheme.print("'");
//	args = cadr(args);
    }
    else {
	gScheme.print("(");
	scm_print(car(arg));

	scm_print(cdr(arg));
	gScheme.print(")");
//	s_save(OP_P1LIST, cdr(args), NIL);
//	args = car(args);
//	s_goto(OP_P0LIST);
    }
}

cell *opexe_print(uint8_t op)
{
    register cell *x;
    static int parLevel = 0;
    
	switch (op) {
	case OP_VALUEPRINT:	/* print evalution result */
		print_flag = 1;
		parLevel = 0;
		args = value;
		//s_save(OP_GC, NIL, NIL);
		s_goto(OP_P0LIST);

	case OP_P0LIST:
		if (!ispair(args)) {
			printatom(args, print_flag);
			return s_return(T);
		} else if (car(args) == QUOTE && ok_abbrev(cdr(args))) {
			gScheme.print("'");
			args = cadr(args);
			s_goto(OP_P0LIST);
		} else {
 			gScheme.print("(");
			parLevel++;
			s_save(OP_P1LIST, cdr(args), NIL);
			args = car(args);
			s_goto(OP_P0LIST);
		}

	case OP_P1LIST:
		if (ispair(args)) {
			s_save(OP_P1LIST, cdr(args), NIL);
			gScheme.print(" ");
			args = car(args);
			s_goto(OP_P0LIST);
		} else {
			if (args != NIL) {
				gScheme.print(" . ");
				printatom(args, print_flag);
			}
			gScheme.print(")");
			parLevel--;
			if (parLevel == 0) {
			    return NIL;
			}
			return s_return(T);
		}
	    
	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;
}


cell *opexe_0(uint8_t op)
{
	register cell *x, *y;

	switch (op) {
	case OP_T0LVL:	/* top level */
	        gScheme.print("\n");
		dump = NIL;
		envir = global_env;
		s_save(OP_VALUEPRINT, NIL, NIL);
		s_save(OP_T1LVL, NIL, NIL);
		if (infp == stdin)
			printf(prompt);
		s_goto(OP_READ);

	case OP_T1LVL:	/* top level */
		code = value;
		s_goto(OP_EVAL);
		
	case OP_EVAL:		/* main part of evalution */
		if (issymbol(code)) {	/* symbol */
			for (x = envir; x != NIL; x = cdr(x)) {
				for (y = car(x); y != NIL; y = cdr(y))
					if (caar(y) == code)
						break;
				if (y != NIL)
					break;
			}
			if (x != NIL) {
				return s_return(cdar(y));
			} else {
				Error_1("Unbounded variable", code);
			}
		} else if (ispair(code)) {
			if (issyntax(x = car(code))) {	/* SYNTAX */
				code = cdr(code);
				s_goto(syntaxnum(x));
			} else {/* first, eval top element and eval arguments */
#ifdef USE_MACRO
				s_save(OP_E0ARGS, NIL, code);
#else
				s_save(OP_E1ARGS, NIL, cdr(code));
#endif
				code = car(code);
				s_goto(OP_EVAL);
			}
		} else {
			return s_return(code);
		}

#ifdef USE_MACRO
	case OP_E0ARGS:	/* eval arguments */
		if (ismacro(value)) {	/* macro expansion */
			s_save(OP_DOMACRO, NIL, NIL);
			args = cons(code, NIL);
			code = value;
			s_goto(OP_APPLY);
		} else {
			code = cdr(code);
			s_goto(OP_E1ARGS);
		}
#endif

	case OP_E1ARGS:	/* eval arguments */
		args = cons(value, args);
		if (ispair(code)) {	/* continue */
			s_save(OP_E1ARGS, args, cdr(code));
			code = car(code);
			args = NIL;
			s_goto(OP_EVAL);
		} else {	/* end */
			args = reverse(args);
			code = car(args);
			args = cdr(args);
			s_goto(OP_APPLY);
		}

	case OP_APPLY:		/* apply 'code' to 'args' */
		if (isproc(code)) {
			s_goto(procnum(code));	/* PROCEDURE */
		} else if (isclosure(code)) {	/* CLOSURE */
			/* make environment */
			envir = cons(NIL, closure_env(code));
			for (x = car(closure_code(code)), y = args;
			     ispair(x); x = cdr(x), y = cdr(y)) {
				if (y == NIL) {
					Error_0("Few arguments");
				} else {
					car(envir) = cons(cons(car(x), car(y)), car(envir));
				}
			}
			if (x == NIL) {
				/*--
				 * if (y != NIL) {
				 * 	Error_0("Many arguments");
				 * }
				 */
			} else if (issymbol(x))
				car(envir) = cons(cons(x, y), car(envir));
			else {
				Error_0("Syntax error in closure");
			}
			code = cdr(closure_code(code));
			args = NIL;
			s_goto(OP_BEGIN);
		} else if (iscontinuation(code)) {	/* CONTINUATION */
			dump = cont_dump(code);
			return s_return(args != NIL ? car(args) : NIL);
		} else {
			Error_0("Illegal function");
		}

#ifdef USE_MACRO
	case OP_DOMACRO:	/* do macro */
		code = value;
		s_goto(OP_EVAL);
#endif

	case OP_LAMBDA:	/* lambda */
		return s_return(mk_closure(code, envir));

	case OP_QUOTE:		/* quote */
		return s_return(car(code));

	case OP_DEF0:	/* define */
		if (ispair(car(code))) {
			x = caar(code);
			code = cons(LAMBDA, cons(cdar(code), cdr(code)));
		} else {
			x = car(code);
			code = cadr(code);
		}
		if (!issymbol(x)) {
			Error_0("Variable is not symbol");
		}
		s_save(OP_DEF1, NIL, x);
		s_goto(OP_EVAL);

	case OP_DEF1:	/* define */
		for (x = car(envir); x != NIL; x = cdr(x))
			if (caar(x) == code)
				break;
		if (x != NIL)
			cdar(x) = value;
		else
			car(envir) = cons(cons(code, value), car(envir));
#ifdef OWN_REPL
		value = code;
		return NIL;
#else
		return s_return(code);
#endif

	case OP_SET0:		/* set! */
		s_save(OP_SET1, NIL, car(code));
		code = cadr(code);
		s_goto(OP_EVAL);

	case OP_SET1:		/* set! */
		for (x = envir; x != NIL; x = cdr(x)) {
			for (y = car(x); y != NIL; y = cdr(y))
				if (caar(y) == code)
					break;
			if (y != NIL)
				break;
		}
		if (x != NIL) {
			cdar(y) = value;
			return s_return(value);
		} else {
			Error_1("Unbounded variable", code);
		}

	case OP_BEGIN:		/* begin */
		if (!ispair(code)) {
			return s_return(code);
		}
		if (cdr(code) != NIL) {
			s_save(OP_BEGIN, NIL, cdr(code));
		}
		code = car(code);
		s_goto(OP_EVAL);

	case OP_IF0:		/* if */
		s_save(OP_IF1, NIL, cdr(code));
		code = car(code);
		s_goto(OP_EVAL);

	case OP_IF1:		/* if */
		if (istrue(value))
			code = car(code);
		else
			code = cadr(code);	/* (if #f 1) ==> () because
						 * car(NIL) = NIL */
		s_goto(OP_EVAL);

	case OP_LET0:		/* let */
		args = NIL;
		value = code;
		code = issymbol(car(code)) ? cadr(code) : car(code);
		s_goto(OP_LET1);

	case OP_LET1:		/* let (caluculate parameters) */
		args = cons(value, args);
		if (ispair(code)) {	/* continue */
			s_save(OP_LET1, args, cdr(code));
			code = cadar(code);
			args = NIL;
			s_goto(OP_EVAL);
		} else {	/* end */
			args = reverse(args);
			code = car(args);
			args = cdr(args);
			s_goto(OP_LET2);
		}

	case OP_LET2:		/* let */
		envir = cons(NIL, envir);
		for (x = issymbol(car(code)) ? cadr(code) : car(code), y = args;
		     y != NIL; x = cdr(x), y = cdr(y))
			car(envir) = cons(cons(caar(x), car(y)), car(envir));
		if (issymbol(car(code))) {	/* named let */
			for (x = cadr(code), args = NIL; x != NIL; x = cdr(x))
				args = cons(caar(x), args);
			x = mk_closure(cons(reverse(args), cddr(code)), envir);
			car(envir) = cons(cons(car(code), x), car(envir));
			code = cddr(code);
			args = NIL;
		} else {
			code = cdr(code);
			args = NIL;
		}
		s_goto(OP_BEGIN);

	case OP_LET0AST:	/* let* */
		if (car(code) == NIL) {
			envir = cons(NIL, envir);
			code = cdr(code);
			s_goto(OP_BEGIN);
		}
		s_save(OP_LET1AST, cdr(code), car(code));
		code = cadaar(code);
		s_goto(OP_EVAL);

	case OP_LET1AST:	/* let* (make new frame) */
		envir = cons(NIL, envir);
		s_goto(OP_LET2AST);

	case OP_LET2AST:	/* let* (caluculate parameters) */
		car(envir) = cons(cons(caar(code), value), car(envir));
		code = cdr(code);
		if (ispair(code)) {	/* continue */
			s_save(OP_LET2AST, args, code);
			code = cadar(code);
			args = NIL;
			s_goto(OP_EVAL);
		} else {	/* end */
			code = args;
			args = NIL;
			s_goto(OP_BEGIN);
		}
	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;
}


cell *opexe_1(uint8_t op)
{
	register cell *x, *y;

	switch (op) {
	case OP_LET0REC:	/* letrec */
		envir = cons(NIL, envir);
		args = NIL;
		value = code;
		code = car(code);
		s_goto(OP_LET1REC);

	case OP_LET1REC:	/* letrec (caluculate parameters) */
		args = cons(value, args);
		if (ispair(code)) {	/* continue */
			s_save(OP_LET1REC, args, cdr(code));
			code = cadar(code);
			args = NIL;
			s_goto(OP_EVAL);
		} else {	/* end */
			args = reverse(args);
			code = car(args);
			args = cdr(args);
			s_goto(OP_LET2REC);
		}

	case OP_LET2REC:	/* letrec */
		for (x = car(code), y = args; y != NIL; x = cdr(x), y = cdr(y))
			car(envir) = cons(cons(caar(x), car(y)), car(envir));
		code = cdr(code);
		args = NIL;
		s_goto(OP_BEGIN);

	case OP_COND0:		/* cond */
		if (!ispair(code)) {
			Error_0("Syntax error in cond");
		}
		s_save(OP_COND1, NIL, code);
		code = caar(code);
		s_goto(OP_EVAL);

	case OP_COND1:		/* cond */
		if (istrue(value)) {
			if ((code = cdar(code)) == NIL) {
				return s_return(value);
			}
			s_goto(OP_BEGIN);
		} else {
			if ((code = cdr(code)) == NIL) {
				return s_return(NIL);
			} else {
				s_save(OP_COND1, NIL, code);
				code = caar(code);
				s_goto(OP_EVAL);
			}
		}

	case OP_DELAY:		/* delay */
		x = mk_closure(cons(NIL, code), envir);
		x->flag |= T_PROMISE;
		return s_return(x);

	case OP_AND0:		/* and */
		if (code == NIL) {
			return s_return(T);
		}
		s_save(OP_AND1, NIL, cdr(code));
		code = car(code);
		s_goto(OP_EVAL);

	case OP_AND1:		/* and */
		if (isfalse(value)) {
			return s_return(value);
		} else if (code == NIL) {
			return s_return(value);
		} else {
			s_save(OP_AND1, NIL, cdr(code));
			code = car(code);
			s_goto(OP_EVAL);
		}

	case OP_OR0:		/* or */
		if (code == NIL) {
			return s_return(F);
		}
		s_save(OP_OR1, NIL, cdr(code));
		code = car(code);
		s_goto(OP_EVAL);

	case OP_OR1:		/* or */
		if (istrue(value)) {
			return s_return(value);
		} else if (code == NIL) {
			return s_return(value);
		} else {
			s_save(OP_OR1, NIL, cdr(code));
			code = car(code);
			s_goto(OP_EVAL);
		}

	case OP_C0STREAM:	/* cons-stream */
		s_save(OP_C1STREAM, NIL, cdr(code));
		code = car(code);
		s_goto(OP_EVAL);

	case OP_C1STREAM:	/* cons-stream */
		args = value;	/* save value to register args for gc */
		x = mk_closure(cons(NIL, code), envir);
		x->flag |= T_PROMISE;
		return s_return(cons(args, x));

#ifdef USE_MACRO
	case OP_0MACRO:	/* macro */
		x = car(code);
		code = cadr(code);
		if (!issymbol(x)) {
			Error_0("Variable is not symbol");
		}
		s_save(OP_1MACRO, NIL, x);
		s_goto(OP_EVAL);

	case OP_1MACRO:	/* macro */
		value->flag |= T_MACRO;
		for (x = car(envir); x != NIL; x = cdr(x))
			if (caar(x) == code)
				break;
		if (x != NIL)
			cdar(x) = value;
		else
			car(envir) = cons(cons(code, value), car(envir));
		return s_return(code);
#endif

	case OP_CASE0:		/* case */
		s_save(OP_CASE1, NIL, cdr(code));
		code = car(code);
		s_goto(OP_EVAL);

	case OP_CASE1:		/* case */
		for (x = code; x != NIL; x = cdr(x)) {
			if (!ispair(y = caar(x)))
				break;
			for ( ; y != NIL; y = cdr(y))
				if (eqv(car(y), value))
					break;
			if (y != NIL)
				break;
		}
		if (x != NIL) {
			if (ispair(caar(x))) {
				code = cdar(x);
				s_goto(OP_BEGIN);
			} else {/* else */
				s_save(OP_CASE2, NIL, cdar(x));
				code = caar(x);
				s_goto(OP_EVAL);
			}
		} else {
			return s_return(NIL);
		}

	case OP_CASE2:		/* case */
		if (istrue(value)) {
			s_goto(OP_BEGIN);
		} else {
			return s_return(NIL);
		}
	case OP_PAPPLY:	/* apply */
		code = car(args);
		args = cadr(args);
		s_goto(OP_APPLY);

	case OP_PEVAL:	/* eval */
		code = car(args);
		args = NIL;
		s_goto(OP_EVAL);

	case OP_CONTINUATION:	/* call-with-current-continuation */
		code = car(args);
		args = cons(mk_continuation(dump), NIL);
		s_goto(OP_APPLY);

	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;
}


cell *opexe_2(uint8_t op)
{
	register cell *x, *y;
	register long v;

	switch (op) {
	case OP_ADD:		/* + */
		for (x = args, v = 0; x != NIL; x = cdr(x))
			v += ivalue(car(x));
		return s_return(mk_number(v));

	case OP_SUB:		/* - */
		for (x = cdr(args), v = ivalue(car(args)); x != NIL; x = cdr(x))
			v -= ivalue(car(x));
		return s_return(mk_number(v));

	case OP_MUL:		/* * */
		for (x = args, v = 1; x != NIL; x = cdr(x))
			v *= ivalue(car(x));
		return s_return(mk_number(v));

	case OP_DIV:		/* / */
		for (x = cdr(args), v = ivalue(car(args)); x != NIL; x = cdr(x)) {
			if (ivalue(car(x)) != 0)
				v /= ivalue(car(x));
			else {
				Error_0("Divided by zero");
			}
		}
		return s_return(mk_number(v));

	case OP_REM:		/* remainder */
		for (x = cdr(args), v = ivalue(car(args)); x != NIL; x = cdr(x)) {
			if (ivalue(car(x)) != 0)
				v %= ivalue(car(x));
			else {
				Error_0("Divided by zero");
			}
		}
		return s_return(mk_number(v));

	case OP_CAR:		/* car */
		if (ispair(car(args))) {
			return s_return(caar(args));
		} else {
			Error_0("Unable to car for non-cons cell");
		}

	case OP_CDR:		/* cdr */
		if (ispair(car(args))) {
			return s_return(cdar(args));
		} else {
			Error_0("Unable to cdr for non-cons cell");
		}

	case OP_CONS:		/* cons */
		cdr(args) = cadr(args);
		return s_return(args);

	case OP_SETCAR:	/* set-car! */
		if (ispair(car(args))) {
			caar(args) = cadr(args);
			return s_return(car(args));
		} else {
			Error_0("Unable to set-car! for non-cons cell");
		}

	case OP_SETCDR:	/* set-cdr! */
		if (ispair(car(args))) {
			cdar(args) = cadr(args);
			return s_return(car(args));
		} else {
			Error_0("Unable to set-cdr! for non-cons cell");
		}

	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;
}


cell *opexe_3(uint8_t op)
{
	register cell *x, *y;

	switch (op) {
	case OP_NOT:		/* not */
		s_retbool(isfalse(car(args)));
	case OP_BOOL:		/* boolean? */
		s_retbool(car(args) == F || car(args) == T);
	case OP_NULL:		/* null? */
		s_retbool(car(args) == NIL);
	case OP_ZEROP:		/* zero? */
		s_retbool(ivalue(car(args)) == 0);
	case OP_POSP:		/* positive? */
		s_retbool(ivalue(car(args)) > 0);
	case OP_NEGP:		/* negative? */
		s_retbool(ivalue(car(args)) < 0);
	case OP_NEQ:		/* = */
		s_retbool(ivalue(car(args)) == ivalue(cadr(args)));
	case OP_LESS:		/* < */
		s_retbool(ivalue(car(args)) < ivalue(cadr(args)));
	case OP_GRE:		/* > */
		s_retbool(ivalue(car(args)) > ivalue(cadr(args)));
	case OP_LEQ:		/* <= */
		s_retbool(ivalue(car(args)) <= ivalue(cadr(args)));
	case OP_GEQ:		/* >= */
		s_retbool(ivalue(car(args)) >= ivalue(cadr(args)));
	case OP_SYMBOL:	/* symbol? */
		s_retbool(issymbol(car(args)));
	case OP_NUMBER:	/* number? */
		s_retbool(isnumber(car(args)));
	case OP_STRING:	/* string? */
		s_retbool(isstring(car(args)));
	case OP_PROC:		/* procedure? */
		/*--
	         * continuation should be procedure by the example
	         * (call-with-current-continuation procedure?) ==> #t
                 * in R^3 report sec. 6.9
	         */
		s_retbool(isproc(car(args)) || isclosure(car(args))
			  || iscontinuation(car(args)));
	case OP_PAIR:		/* pair? */
		s_retbool(ispair(car(args)));
	case OP_EQ:		/* eq? */
		s_retbool(car(args) == cadr(args));
	case OP_EQV:		/* eqv? */
		s_retbool(eqv(car(args), cadr(args)));
	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;
}


cell *opexe_4(uint8_t op)
{
	register cell *x, *y;

	switch (op) {
	case OP_FORCE:		/* force */
		code = car(args);
		if (code->flag & T_PROMISE) {
			args = NIL;
			s_goto(OP_APPLY);
		} else {
			return s_return(code);
		}

	case OP_WRITE:		/* write */
		print_flag = 1;
		args = car(args);
		s_goto(OP_P0LIST);

	case OP_DISPLAY:	/* display */
		print_flag = 0;
		args = car(args);
		s_goto(OP_P0LIST);

	case OP_NEWLINE:	/* newline */
		gScheme.print("\n");
		return s_return(T);

	case OP_ERR0:	/* error */
		if (!isstring(car(args))) {
			Error_0("error -- first argument must be string");
		}
		gScheme.print("Error: ");
		gScheme.print(strvalue(car(args)));
		args = cdr(args);
		s_goto(OP_ERR1);

	case OP_ERR1:	/* error */
		gScheme.print(" ");
		if (args != NIL) {
			s_save(OP_ERR1, cdr(args), NIL);
			args = car(args);
			print_flag = 1;
			s_goto(OP_P0LIST);
		} else {
			gScheme.print("\n");
			flushinput();
			//s_goto(OP_T0LVL);
			return NIL;
		}

	case OP_REVERSE:	/* reverse */
		return s_return(reverse(car(args)));

	case OP_APPEND:	/* append */
		return s_return(append(car(args), cadr(args)));

	case OP_QUIT:		/* quit */
		return (NIL);

	case OP_GC:		/* gc */
		gc(NIL, NIL);
		return s_return(T);

	case OP_GCVERB:		/* gc-verbose */
	{	int	was = gc_verbose;
		
		gc_verbose = (car(args) != F);
		s_retbool(was);
	}

	case OP_NEWSEGMENT:	/* new-segment */
		if (!isnumber(car(args))) {
			Error_0("new-segment -- argument must be number");
		}
		gScheme.print("allocate new segments\n");
		
		alloc_cellseg((int) ivalue(car(args)));
		return s_return(T);
	}
}

cell *opexe_6(uint8_t op)
{
	register cell *x, *y;
	register long v;
	char	buffer[32];

	switch (op) {
	case OP_LIST_LENGTH:	/* list-length */	/* a.k */
		for (x = car(args), v = 0; ispair(x); x = cdr(x))
			++v;
		return s_return(mk_number(v));
		
	case OP_ASSQ:		/* assq */	/* a.k */
		x = car(args);
		for (y = cadr(args); ispair(y); y = cdr(y)) {
			if (!ispair(car(y))) {
				Error_0("Unable to handle non pair element");
			}
			if (x == caar(y))
				break;
		}
		if (ispair(y)) {
			return s_return(car(y));
		} else {
			return s_return(F);
		}
		
	case OP_GET_CLOSURE:	/* get-closure-code */	/* a.k */
		args = car(args);
		if (args == NIL) {
			return s_return(F);
		} else if (isclosure(args)) {
			return s_return(cons(LAMBDA, closure_code(value)));
#ifdef USE_MACRO
		} else if (ismacro(args)) {
			return s_return(cons(LAMBDA, closure_code(value)));
#endif
		} else {
			return s_return(F);
		}
	case OP_CLOSUREP:		/* closure? */
		/*
		 * Note, macro object is also a closure.
		 * Therefore, (closure? <#MACRO>) ==> #t
		 */
		if (car(args) == NIL) {
		    return s_return(F);
		}
		s_retbool(isclosure(car(args)));
#ifdef USE_MACRO
	case OP_MACROP:		/* macro? */
		if (car(args) == NIL) {
		    return s_return(F);
		}
		s_retbool(ismacro(car(args)));
#endif
	default:
		sprintf(strbuff, "%d is illegal operator", operator);
		Error_0(strbuff);
	}
	return T;	/* NOTREACHED */
}


/* kernel of this intepreter */
cell *Eval_Cycle(OpCodes op)
{
	operator = op;
	for (;;) {
	    printf("[%s]", OpNames[operator]);
	    fflush(stdout);
	    if ((*dispatch_table[operator])(operator) == NIL)
		return NIL;
	}
}

/* ========== Initialization of internal keywords ========== */

void mk_syntax(unsigned short op, const char *name)
{
	cell *x;

	x = cons(mk_string(name), NIL);
	x->flag = (T_SYNTAX | T_SYMBOL);
	syntaxnum(x) = op;
	oblist = cons(x, oblist);
}

void mk_proc(unsigned short op, const char *name)
{
	cell *x, *y;

	x = mk_symbol(name);
	y = get_cell(NIL, NIL);
	y->flag = (T_PROC | T_ATOM);
	ivalue(y) = (long) op;
	car(global_env) = cons(cons(x, y), car(global_env));
}


void init_vars_global()
{
	cell *x;

	/* init input/output file */
	infp = stdin;
	/* init NIL */
	NIL->flag = (T_ATOM | MARK);
	car(NIL) = cdr(NIL) = NIL;
	/* init T */
	T->flag = (T_ATOM | MARK);
	car(T) = cdr(T) = T;
	/* init F */
	F->flag = (T_ATOM | MARK);
	car(F) = cdr(F) = F;
	/* init global_env */
	global_env = cons(NIL, NIL);
	/* init else */
	x = mk_symbol("else");
	car(global_env) = cons(cons(x, T), car(global_env));
}

void init_syntax()
{
	/* init syntax */
	mk_syntax(OP_LAMBDA, "lambda");
	mk_syntax(OP_QUOTE, "quote");
	mk_syntax(OP_DEF0, "define");
	mk_syntax(OP_IF0, "if");
	mk_syntax(OP_BEGIN, "begin");
	mk_syntax(OP_SET0, "set!");
	mk_syntax(OP_LET0, "let");
	mk_syntax(OP_LET0AST, "let*");
	mk_syntax(OP_LET0REC, "letrec");
	mk_syntax(OP_COND0, "cond");
	mk_syntax(OP_DELAY, "delay");
	mk_syntax(OP_AND0, "and");
	mk_syntax(OP_OR0, "or");
	mk_syntax(OP_C0STREAM, "cons-stream");
#ifdef USE_MACRO
	mk_syntax(OP_0MACRO, "macro");
#endif
	mk_syntax(OP_CASE0, "case");
}


void init_procs()
{
	/* init procedure */
	mk_proc(OP_PEVAL, "eval");
	mk_proc(OP_PAPPLY, "apply");
	mk_proc(OP_CONTINUATION, "call-with-current-continuation");
	mk_proc(OP_FORCE, "force");
	mk_proc(OP_CAR, "car");
	mk_proc(OP_CDR, "cdr");
	mk_proc(OP_CONS, "cons");
	mk_proc(OP_SETCAR, "set-car!");
	mk_proc(OP_SETCDR, "set-cdr!");
	mk_proc(OP_ADD, "+");
	mk_proc(OP_SUB, "-");
	mk_proc(OP_MUL, "*");
	mk_proc(OP_DIV, "/");
	mk_proc(OP_REM, "remainder");
	mk_proc(OP_NOT, "not");
	mk_proc(OP_BOOL, "boolean?");
	mk_proc(OP_SYMBOL, "symbol?");
	mk_proc(OP_NUMBER, "number?");
	mk_proc(OP_STRING, "string?");
	mk_proc(OP_PROC, "procedure?");
	mk_proc(OP_PAIR, "pair?");
	mk_proc(OP_EQV, "eqv?");
	mk_proc(OP_EQ, "eq?");
	mk_proc(OP_NULL, "null?");
	mk_proc(OP_ZEROP, "zero?");
	mk_proc(OP_POSP, "positive?");
	mk_proc(OP_NEGP, "negative?");
	mk_proc(OP_NEQ, "=");
	mk_proc(OP_LESS, "<");
	mk_proc(OP_GRE, ">");
	mk_proc(OP_LEQ, "<=");
	mk_proc(OP_GEQ, ">=");
	mk_proc(OP_READ, "read");
	mk_proc(OP_WRITE, "write");
	mk_proc(OP_DISPLAY, "display");
	mk_proc(OP_NEWLINE, "newline");
	mk_proc(OP_ERR0, "error");
	mk_proc(OP_REVERSE, "reverse");
	mk_proc(OP_APPEND, "append");
	mk_proc(OP_GC, "gc");
	mk_proc(OP_GCVERB, "gc-verbose");
	mk_proc(OP_NEWSEGMENT, "new-segment");
	mk_proc(OP_LIST_LENGTH, "list-length");	/* a.k */
	mk_proc(OP_ASSQ, "assq");	/* a.k */
	mk_proc(OP_GET_CLOSURE, "get-closure-code");	/* a.k */
	mk_proc(OP_CLOSUREP, "closure?");	/* a.k */
#ifdef USE_MACRO
	mk_proc(OP_MACROP, "macro?");	/* a.k */
#endif
	mk_proc(OP_QUIT, "quit");
}


/* initialize several globals */
void init_globals()
{
	init_vars_global();
	init_syntax();
	init_procs();
	/* intialization of global pointers to special symbols */
	LAMBDA = mk_symbol("lambda");
	QUOTE = mk_symbol("quote");
}

/* ========== Error ==========  */

void FatalError(char *fmt)
{
	fprintf(stderr, "Fatal error: ");
	fprintf(stderr, fmt);
	fprintf(stderr, "\n");
	exit(1);
}

//------------------------------------------------------------------------------
void print(const char* msg)
{
    printf("%s", msg);
}

int main()
{
    gScheme.print = print;
	
    printf("Mini-Scheme Interpreter V0.85j2.\n");
    init_scheme();

    if ((infp = fopen("init.scm", "r")) == NULL) {
		infp = stdin;
		printf("can not open init.scm");
    }

    // repl
    //
#ifdef OWN_REPL
    while (1) {
	dump = NIL;
	envir = global_env;

	if (infp == stdin)
	    print("\nscm>");

	printf("\nREAD------------\n");
	Eval_Cycle(OP_READ);
	code = value;

	//Eval_Cycle(OP_EVAL);

	printf("\nPRINT------------\n");
	//Eval_Cycle(OP_VALUEPRINT);
	scm_print(value);
    }
#else
    Eval_Cycle(OP_T0LVL);    
#endif
    return 0;
}
