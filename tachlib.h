#ifndef TACH_LIBRARY_HEADER
#define TACH_LIBRARY_HEADER
#include "tach.h"

tach_obj *tach_lib_pass(tach_obj *cap, tach_obj *raw_arg) {
    return tach_pair(cap, tach_function(tach_lib_pass));
}

tach_obj *tach_lib_newline(tach_obj *cap, tach_obj *raw_arg) {
    printf("\n");
    return tach_pair(tach_nil(), tach_nil());
}

tach_obj *tach_lib_op_add(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    if (cap->kind == TACH_OBJ_NIL) {
        return tach_pair(arg, tach_function(tach_lib_op_add));
    }
    double result = arg->value.number + cap->value.number;
    return tach_pair(tach_number(result), tach_function(tach_lib_op_add));
}

tach_obj *tach_lib_op_mul(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    if (cap->kind == TACH_OBJ_NIL) {
        return tach_pair(arg, tach_function(tach_lib_op_mul));
    }
    double result = arg->value.number * cap->value.number;
    return tach_pair(tach_number(result), tach_function(tach_lib_op_mul));
}

tach_obj *tach_lib_op_div_help_2(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    double result = cap->value.number / arg->value.number; 
    return tach_pair(tach_number(result), tach_function(tach_lib_op_div_help_2));
}

tach_obj *tach_lib_op_div_help_1(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    double result = 1/cap->value.number / arg->value.number; 
    return tach_pair(tach_number(result), tach_function(tach_lib_op_div_help_2));
}

tach_obj *tach_lib_op_div(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    return tach_pair(tach_number(1/arg->value.number), tach_function(tach_lib_op_div_help_1));
}

tach_obj *tach_lib_op_sub_help_2(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    double result = cap->value.number - arg->value.number; 
    return tach_pair(tach_number(result), tach_function(tach_lib_op_sub_help_2));
}

tach_obj *tach_lib_op_sub_help_1(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    double result = -cap->value.number - arg->value.number; 
    return tach_pair(tach_number(result), tach_function(tach_lib_op_sub_help_2));
}

tach_obj *tach_lib_op_sub(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    return tach_pair(tach_number(-arg->value.number), tach_function(tach_lib_op_sub_help_1));
}

tach_obj *tach_lib_loop(tach_obj *cap, tach_obj *raw_arg) {
    if (cap->kind == TACH_OBJ_NIL) {
        tach_obj *arg = tach_walk(raw_arg);
        return tach_pair(arg, tach_function(tach_lib_loop));
    }
    else {
        double maxv = cap->value.number;
        tach_obj *ret = tach_nil();
        for (double i = 0; i < maxv; i ++) {
            ret = tach_walk(raw_arg);
        }
        return tach_pair(ret, tach_function(tach_lib_loop));
    }
}

tach_obj *tach_lib_print(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    tach_obj_println(arg);
    return tach_pair(tach_nil(), tach_function(tach_lib_print));
}

tach_obj *tach_lib_help_def_name(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    tach_set_global(cap, arg);
    return tach_pair(tach_nil(), tach_function(tach_lib_pass));
}

tach_obj *tach_lib_help_def_function(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *obj = tach_quote(tach_pair(raw_arg, cap->value.pair->cdr));
    tach_set_global(cap->value.pair->car, obj);
    return tach_pair(tach_nil(), tach_function(tach_lib_pass));
}

tach_obj *tach_lib_cond_help_was_not_nil(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    return tach_pair(arg, tach_function(tach_lib_pass));
    
}

tach_obj *tach_lib_cond_help_was_nil(tach_obj *cap, tach_obj *raw_arg) {
    return tach_pair(tach_nil(), tach_function(tach_lib_cond_help_was_not_nil));
}

tach_obj *tach_lib_cond(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    // printf("%d\n", arg->kind != TACH_OBJ_NIL);
    // tach_obj_println(arg);
    if (arg->kind == TACH_OBJ_NIL) {
        return tach_pair(tach_nil(), tach_function(tach_lib_cond_help_was_nil));
    }
    else {
        return tach_pair(tach_nil(), tach_function(tach_lib_cond_help_was_not_nil));
    }
}

tach_obj *tach_lib_op_equal(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    if (cap->kind == TACH_OBJ_NIL) {
        return tach_pair(arg, tach_function(tach_lib_op_equal));
    }
    else if (arg->kind != TACH_OBJ_NIL) {
        if (!tach_equals(cap, arg)) {
            return tach_pair(tach_nil(), tach_function(tach_lib_pass));
        }
        else {
            return tach_pair(arg, tach_function(tach_lib_op_equal));
        }
    }
    else {
        return tach_number(0);
    }
}

tach_obj *tach_lib_or(tach_obj *cap, tach_obj *raw_arg) {
    tach_obj *arg = tach_walk(raw_arg);
    if (arg->kind == TACH_OBJ_NIL) {
        return tach_pair(arg, tach_function(tach_lib_or));
    }
    else {
        return tach_pair(arg, tach_function(tach_lib_pass));
    }
}

tach_obj *tach_lib_def(tach_obj *cap, tach_obj *raw_arg) {
    if (raw_arg->kind == TACH_OBJ_PAIR) {
        return tach_pair(raw_arg, tach_function(tach_lib_help_def_function));
    }
    else {
        return tach_pair(raw_arg, tach_function(tach_lib_help_def_name));
    }
}

#endif