#if !defined(num_h)
#define num_h

#include <stdio.h>
#include <stdbool.h>

typedef struct num num;

void nm_put(FILE *out, num *nm);

num *nm_free(num *nm);
num *nm_create(unsigned long v);
num *nm_parse(const char *value);

num *nm_add(num *a, num *b);
num *nm_sub(num *a, num *b);
num *nm_mult(num *a, num *b);
num *nm_div(num *a, num *b, num **modulus);

num *nm_gcd(num *a, num *b);

bool nm_eq(num *a, num *b);
bool nm_eq_1(num *n);
bool nm_leq(num *a, num *b);

#endif
