#if !defined(frac_h)
#define frac_h

    #include "num.h"

    typedef struct frac frac;

    void fr_put(FILE *out, frac* fr);

    frac *fr_free(frac *fr);
    frac *fr_create(num *n, num *d, bool negative);
    frac *fr_create_simple(long n, unsigned long d);

    num *fr_num(frac *fr);
    num *fr_denom(frac *fr);

    frac *fr_neg(frac *fr);
    frac *fr_inv(frac *fr);

    frac *fr_add(frac *a, frac *b);
    frac *fr_sub(frac *a, frac *b);
    frac *fr_mult(frac *a, frac *b);
    frac *fr_div(frac *a, frac *b);

    bool fr_eq(frac *a, frac *b);
    bool fr_leq(frac *a, frac *b);

#endif
