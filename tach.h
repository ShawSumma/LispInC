
#ifndef TACH_HEADER
#define TACH_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gc.h>

#define malloc(N) GC_MALLOC(N)

struct tach_obj;
typedef struct tach_obj tach_obj;

struct tach_obj_pair;
typedef struct tach_obj_pair tach_obj_pair;

typedef tach_obj *(*tach_functionptr)(tach_obj *,tach_obj *);

void tach_obj_print(tach_obj *);
void tach_obj_println(tach_obj *);
void tach_trim_file(FILE *);
tach_obj *tach_deep_copy(tach_obj *);
void tach_main(char *);
char tach_string_equals(char *, char *);
char tach_equals(tach_obj *, tach_obj *);
void tach_new_global(tach_obj *, tach_obj *);
void tach_set_global(tach_obj *, tach_obj *);
void tach_new_string_global(char *, tach_obj *);
tach_obj *tach_get_global(tach_obj *);
tach_obj *tach_string_global(char *);
tach_obj *tach_walk(tach_obj *);
tach_obj *tach_nil();
tach_obj *tach_pair(tach_obj *, tach_obj *);
tach_obj *tach_function(tach_functionptr);
tach_obj *tach_number(double);
tach_obj *tach_quote(tach_obj *);
tach_obj *tach_string(char *);

enum tach_obj_kind {
    TACH_OBJ_NIL = 1,
    TACH_OBJ_NUMBER,
    TACH_OBJ_PAIR,
    TACH_OBJ_NAME,
    TACH_OBJ_QUOTE,
    TACH_OBJ_FUNCTION,
    TACH_OBJ_STRING,
};
typedef enum tach_obj_kind tach_obj_kind;

union tach_obj_value {
    double number;
    char *string;
    tach_obj *quote;
    tach_obj_pair *pair;
    tach_functionptr function;
};
typedef union tach_obj_value tach_obj_value;

struct tach_obj_pair {
    tach_obj *car;
    tach_obj *cdr;
};

struct tach_obj {
    tach_obj_kind kind;
    tach_obj_value value;
};

#include "tachlib.h"
#include "interp.h"
#endif