#pragma once

typedef struct scm {
    void (*print)(const char*);
    
} scm;

typedef struct cell cell;
typedef cell *(*foreign_func)(cell*);

/* cell structure */
struct cell {
	unsigned short flag;
	union {
		struct {
			char   *_svalue;
			short   _keynum;
		} _string;
	        long    _ivalue;
	        foreign_func ffunc;
		struct {
			cell *_car;
			cell *_cdr;
		} _cons;
	} obj;
};


typedef cell *(*ScmFunc)(uint8_t);

