#ifndef __FLOAT_ARITH_H
#define __FLOAT_ARITH_H

#include <inttypes.h>

#define FRACTION (1 << 14)

int i_sub_f(int i, int f);
int addInt(int f, int i);  // f_add_i
int f_sub_i(int f, int i);
int mulFloat(int f1, int f2);  // f_mul_f
int f_div_f(int f1, int f2);
int f_add_f(int f1, int f2);

//삭제 : f_sub_f, f_div_i, i_mul_f
#endif
