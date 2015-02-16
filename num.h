#if !defined(num_h)
#define num_h

#include <stdio.h>
#include <stdbool.h>

typedef struct num num;


#pragma mark - output numbers

void nm_put(FILE *out, num *nm);


#pragma mark - memory handling

num *nm_free(num *nm);
num *nm_create(unsigned long v);
num *nm_parse(const char *value);


#pragma mark - adding and subtracting

num *nm_add(num *a, num *b);
num *nm_sub(num *a, num *b);


#pragma mark - multiplication and divison

num *nm_mult(num *a, num *b);
num *nm_div(num *a, num *b, num **modulus);


#pragma mark - comparison

bool nm_eq(num *a, num *b);
bool nm_leq(num *a, num *b);


#endif
