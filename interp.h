#ifndef TACH_INTERP_HEADER
#define TACH_INTERP_HEADER
#include "tach.h"
tach_obj *globals;

long tach_string_size(char *str) {
    long ret = 0;
    while (str[ret]) {
        ret ++;
    }
    return ret;
}

char tach_string_equals(char *first, char *second) {
    long fl = tach_string_size(first);
    long sl = tach_string_size(second);
    if (fl != sl) {
        return 0;
    }
    while (fl > 0) {
        fl --;
        if (first[fl] != second[fl]) {
            return 0;
        }
    }
    return 1;
}

char tach_equals(tach_obj *first, tach_obj *second) {
    if (first->kind != second->kind) {
        return 0;
    }
    switch(first->kind) {
    case TACH_OBJ_PAIR: {
        return tach_equals(first->value.pair->car, second->value.pair->car)
            && tach_equals(first->value.pair->cdr, second->value.pair->cdr);
    }
    case TACH_OBJ_STRING: {
        return tach_string_equals(first->value.string, second->value.string);
    }
    case TACH_OBJ_QUOTE: {
        return tach_equals(first->value.quote, second->value.quote);
    }
    case TACH_OBJ_NAME: {
        return tach_string_equals(first->value.string, second->value.string);
    }
    case TACH_OBJ_NUMBER: {
        return first->value.number == second->value.number;
    }
    case TACH_OBJ_FUNCTION: {
        return 0;
    }
    case TACH_OBJ_NIL: {
        return 1;
    }
    }
}

void tach_new_global(tach_obj *key, tach_obj *value) {

    tach_obj *keyvalue = malloc(sizeof(tach_obj));
    keyvalue->kind = TACH_OBJ_PAIR;
    keyvalue->value.pair = malloc(sizeof(tach_obj_pair));
    keyvalue->value.pair->car = key;
    keyvalue->value.pair->cdr = value;
    keyvalue = tach_deep_copy(keyvalue);


    tach_obj *oldglobals = globals;
    globals = malloc(sizeof(tach_obj));
    globals->kind = TACH_OBJ_PAIR;
    globals->value.pair = malloc(sizeof(tach_obj_pair));
    globals->value.pair->car = keyvalue;
    globals->value.pair->cdr = oldglobals;
}

void tach_new_string_global(char *str, tach_obj *value) {
    tach_obj *key = malloc(sizeof(tach_obj));
    key->kind = TACH_OBJ_NAME;
    long size = tach_string_size(str);
    key->value.string = malloc(size + 1);
    while (size > 0) {
        size --;
        key->value.string[size] = str[size];
    }
    tach_new_global(key, value);
}

void tach_set_global(tach_obj *key, tach_obj *value) {
    tach_obj *curglobals = globals;
    while (curglobals->kind == TACH_OBJ_PAIR) {
        tach_obj *pair = curglobals->value.pair->car;
        if (tach_equals(key, pair->value.pair->car)) {
            pair->value.pair->cdr = value;
            return;
        }
        curglobals = curglobals->value.pair->cdr;
    }
    tach_new_global(key, value);
}

tach_obj *tach_get_global(tach_obj *key) {
    tach_obj *curglobals = globals;
    while (curglobals->kind == TACH_OBJ_PAIR) {
        tach_obj *pair = curglobals->value.pair->car;
        if (tach_equals(key, pair->value.pair->car)) {
            return tach_deep_copy(pair->value.pair->cdr);
        }
        curglobals = curglobals->value.pair->cdr;
    }
    return tach_deep_copy(curglobals);
}

tach_obj *tach_get_string_global(char *str) {
    tach_obj *key = malloc(sizeof(tach_obj));
    key->kind = TACH_OBJ_NAME;
    long size = tach_string_size(str);
    key->value.string = malloc(size + 1);
    while (size > 0) {
        size --;
        key->value.string[size] = str[size];
    }
    return tach_get_global(key);
}

void tach_init() {
    GC_INIT();
    globals = malloc(sizeof(tach_obj));
    globals->kind = TACH_OBJ_NIL;

    tach_new_string_global("newline", tach_function(tach_lib_newline));
    tach_new_string_global("print", tach_function(tach_lib_print));
    tach_new_string_global("loop", tach_function(tach_lib_loop));
    tach_new_string_global("def", tach_function(tach_lib_def));
    tach_new_string_global("cond", tach_function(tach_lib_cond));
    tach_new_string_global("or", tach_function(tach_lib_or));
    tach_new_string_global("=", tach_function(tach_lib_op_equal));
    tach_new_string_global("+", tach_function(tach_lib_op_add));
    tach_new_string_global("*", tach_function(tach_lib_op_mul));
    tach_new_string_global("/", tach_function(tach_lib_op_div));
    tach_new_string_global("-", tach_function(tach_lib_op_sub));
}

void tach_obj_print(tach_obj *obj) {
    switch (obj->kind) {
    case TACH_OBJ_STRING: { 
        printf("\"%s\"", obj->value.string);
        break;
    }
    case TACH_OBJ_NAME: { 
        printf("%s", obj->value.string);
        break;
    }
    case TACH_OBJ_QUOTE: {
        printf("\'");
        tach_obj_print(obj->value.quote);
        break;
    }
    case TACH_OBJ_NIL: {
        printf("Nil");
        break;
    }
    case TACH_OBJ_NUMBER: {
        double num = obj->value.number;
        if (fmod(num, 1) == 0) {
            long lnum = num;
            printf("%ld", lnum);
        }
        else {
            printf("%lf", num);
        }
        break;
    }
    case TACH_OBJ_PAIR: {
        printf("(");
        char begin = 0;
        while (obj->kind != TACH_OBJ_NIL) {
            if (begin == 0) {
                begin = 1;
            }
            else {
                printf(" ");
            }
            tach_obj_print(obj->value.pair->car);
            obj = obj->value.pair->cdr;
        }
        if (obj->kind != TACH_OBJ_NIL) {
            tach_obj_print(obj);
        }
        printf(")");
        break;
    }
    case TACH_OBJ_FUNCTION: {
        if (sizeof(void *) == sizeof(unsigned long)) {
            printf("(function %lu)", (unsigned long) obj);
        }
        else if (sizeof(void *) == sizeof(unsigned int)) {
            printf("(function %d)", (unsigned int) obj);
        }
        else {
            printf("(function Nil)");
        }
        break;
    }
    }
}

void tach_obj_println(tach_obj *obj) {
    tach_obj_print(obj);
    printf("\n");
}

void tach_trim_file(FILE *file) {
    char got = fgetc(file);
    while (got == ' ' || got == '\t' || got == '\n' || got == '\r') {
        got = fgetc(file);
    }
    ungetc(got, file);
}

char tach_peek(FILE *f) {
    char ret = fgetc(f);
    ungetc(ret, f);
    return ret;
}

tach_obj *tach_read(FILE *file) {
    // printf("%c\n", tach_peek(file));
    tach_obj *obj = malloc(sizeof(tach_obj));
    obj->kind = TACH_OBJ_NIL;
    tach_trim_file(file);
    char got = fgetc(file);
    if (got == EOF) {
        return obj;
    }
    if (got == '(') {
        tach_obj *end = obj;
        while (got != ')') {
            tach_trim_file(file);
            end->kind = TACH_OBJ_PAIR;
            end->value.pair = malloc(sizeof(tach_obj_pair));
            end->value.pair->car = tach_read(file);
            end->value.pair->cdr = malloc(sizeof(tach_obj));
            end->value.pair->cdr->kind = TACH_OBJ_NIL;
            end = end->value.pair->cdr;
            got = tach_peek(file);
        }
        fgetc(file);
    }
    else if (got >= '0' && got <= '9') {
        obj->kind = TACH_OBJ_NUMBER;
        obj->value.number = 0;
        unsigned char count = 0;
        do {
            obj->value.number *= 10;
            obj->value.number += got - '0';
            got = fgetc(file);
            count ++;
        } while (got >= '0' && got <= '9');
        ungetc(got, file);
    }
    else if (got == '"') {
        char buf[80];
        char ind = 0;
        do {
            got = fgetc(file);
            buf[ind] = got;
            ind ++;
        } while (got != '"');
        ind --;
        buf[ind] = '\0';
        obj = tach_string(buf);
    }
    else {
        char str[80];
        char ind = 0;
        while (!(got >= '0' && got <= '9')
            && got != '\n' && got != '\t' && got != ' ' && got != '\r'
            && got != '(' && got != ')') {

            str[ind] = got;
            got = fgetc(file);
            ind ++;
        }
        ungetc(got, file);
        obj->kind = TACH_OBJ_NAME;
        obj->value.string = malloc(sizeof(char)*ind);
        while (ind > 0) {
            ind --;
            obj->value.string[ind] = str[ind];
        }
        return obj;
    }
    return obj;
}

tach_obj *tach_number(double d) {
    tach_obj *ret = malloc(sizeof(tach_obj));
    ret->kind = TACH_OBJ_NUMBER;
    ret->value.number = d;
    return ret;
}

tach_obj *tach_nil() {
    tach_obj *ret = malloc(sizeof(tach_obj));
    ret->kind = TACH_OBJ_NIL;
    return ret;
}

tach_obj *tach_string(char *str) {
    long size = tach_string_size(str);
    tach_obj *ret = malloc(sizeof(tach_obj));
    ret->kind = TACH_OBJ_STRING;
    ret->value.string = malloc(sizeof(char) * (size+1));
    while (size > 0) {
        size --;
        ret->value.string[size] = str[size];
    }
    return ret;
}

tach_obj *tach_function(tach_functionptr ptr) {
    tach_obj *ret = malloc(sizeof(tach_obj));
    ret->kind = TACH_OBJ_FUNCTION;
    ret->value.function = ptr;
    return ret;
}

tach_obj *tach_quote(tach_obj *val) {
    tach_obj *ret = malloc(sizeof(tach_obj));
    ret->kind = TACH_OBJ_QUOTE;
    ret->value.quote = val;
    return ret;
}

tach_obj *tach_pair(tach_obj *first, tach_obj *second) {
    tach_obj *ret = malloc(sizeof(tach_obj));
    ret->kind = TACH_OBJ_PAIR;
    ret->value.pair = malloc(sizeof(tach_pair));
    ret->value.pair->car = tach_deep_copy(first);
    ret->value.pair->cdr = tach_deep_copy(second);
    return ret;
}

tach_obj *tach_deep_copy(tach_obj *val) {
    switch (val->kind) {
    case TACH_OBJ_NIL: {
        tach_obj *obj = malloc(sizeof(tach_obj));
        obj->kind = TACH_OBJ_NIL;
        return obj;
    }
    case TACH_OBJ_STRING: {
        tach_obj *obj = malloc(sizeof(tach_obj));
        obj->kind = TACH_OBJ_STRING;
        char *str = val->value.string;
        long size = tach_string_size(str);
        obj->value.string = malloc(size + 1);
        while (size > 0) {
            size --;
            obj->value.string[size] = str[size];
        }
        return obj;
    }
    case TACH_OBJ_NAME: {
        tach_obj *obj = malloc(sizeof(tach_obj));
        obj->kind = TACH_OBJ_NAME;
        char *str = val->value.string;
        long size = tach_string_size(str);
        obj->value.string = malloc(size + 1);
        while (size > 0) {
            size --;
            obj->value.string[size] = str[size];
        }
        return obj;
    }
    case TACH_OBJ_QUOTE: {
        tach_obj *obj = malloc(sizeof(tach_obj));
        obj->kind = TACH_OBJ_QUOTE;
        obj->value.quote = tach_deep_copy(val->value.quote);
        return obj;
    }
    case TACH_OBJ_FUNCTION: {
        tach_obj *obj = malloc(sizeof(tach_obj));
        obj->kind = TACH_OBJ_FUNCTION;
        obj->value.function = val->value.function;
        return obj;
    }
    case TACH_OBJ_NUMBER: {
        tach_obj *obj = malloc(sizeof(tach_obj));
        obj->kind = TACH_OBJ_NUMBER;
        obj->value.function = val->value.function;
        return obj;
    }
    case TACH_OBJ_PAIR: {
        tach_obj *obj = malloc(sizeof(tach_obj));
        obj->kind = TACH_OBJ_PAIR;
        obj->value.pair = malloc(sizeof(tach_obj_pair));
        obj->value.pair->car = tach_deep_copy(val->value.pair->car);
        obj->value.pair->cdr = tach_deep_copy(val->value.pair->cdr);
        return obj;
    }
    }
    tach_obj *obj = malloc(sizeof(tach_obj));
    obj->kind = TACH_OBJ_NIL;
    return obj;
}

tach_obj *tach_walk(tach_obj *obj) {
    switch (obj->kind) {
    case TACH_OBJ_NAME: {
        return tach_get_global(obj);
    }
    case TACH_OBJ_PAIR: {
        tach_obj *prefnobj = tach_walk(obj->value.pair->car);
        tach_obj *ret;
        if (prefnobj->kind == TACH_OBJ_QUOTE) {
            tach_obj *dequoted = prefnobj->value.quote;
            tach_obj *fnobj = dequoted->value.pair->car;
            tach_obj *args = dequoted->value.pair->cdr;
            tach_obj *original = globals;
            obj = obj->value.pair->cdr;
            while (args->kind == TACH_OBJ_PAIR) {
                tach_obj *curarg = tach_walk(obj->value.pair->car);
                tach_new_global(args->value.pair->car, curarg);
                args = args->value.pair->cdr;
                obj = obj->value.pair->cdr;
            }
            ret = tach_walk(fnobj);
            globals = original;
        }
        else {
            tach_obj *fnobj = prefnobj;
            obj = obj->value.pair->cdr;
            tach_obj *cap = tach_nil();
            while (obj->kind == TACH_OBJ_PAIR) {
                tach_obj *arg = obj->value.pair->car;
                tach_functionptr fn = fnobj->value.function;
                tach_obj *result = fn(cap, arg);
                cap = result->value.pair->car;
                fnobj = result->value.pair->cdr;
                obj = obj->value.pair->cdr;
            }
            ret = cap;
        }
        return ret;
    }
    default: {
        return tach_deep_copy(obj);
    }
    }
}

void tach_main(char *name) {
    tach_init();
    FILE *f = fopen(name, "r");
    while (tach_peek(f) != EOF) {
        tach_obj *obj1 = tach_read(f);
        // tach_obj_println(obj1);
        tach_walk(obj1);
    }
    fclose(f);
}

#endif