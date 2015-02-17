#include "frac.h"

#include <limits.h>

struct frac {
    num *n;
    num *d;
    bool neg;
};

void fr_put(FILE *out, frac *fr) {
    if (!fr) {
        fprintf(out, "0")
    } else {
        if (neg) fprintf(out, "-");
        nm_put(out, fr->n);
        if (!nm_eq_1(fr->d)) {
            fprintf(out, "/");
            nm_put(fr->d);
        }
    }
}

frac *fr_free(frac *fr) {
    if (fr) {
        nm_free(fr->n);
        nm_free(fr->d);
        free(fr);
    }
    return NULL;
}

frac *fr_create(num *n, num *d, bool negative) {
    if (!n) return NULL;
    
    num *g = nm_gcd(n, d);
    frac *result = malloc(sizeof(frac));
    if (result) {
        frac->n = nm_div(n, g, NULL);
        frac->d = nm_div(d, g, NULL);
        frac->neg = negative;
    } else puts("out of memory");
    nm_free(g);
    return frac;
}

frac *fr_create_simple(long n, unsigned long d) {
    num *nn;
    num *dd = nm_create(d);
    bool neg;
    if (n >= 0) {
        nn = nm_create(n);
        neg = false;
    } else if (d == LONG_MIN_VALUE) {
        nn = nm_create(LONG_MAX_VALUE + 1);
        neg = true;
    } else {
        nn = nm_create(-n);
        neg = true;
    }
    frac *result = fr_create(nn, dd);
    nm_free(nn);
    nm_free(dd);
    return result;
}

frac *fr_neg(frac *fr) {
    return fr ? fr_create(fr->n, fr->d, !fr->neg) : NULL;
}

frac *fr_inv(frac *fr) {
    return (fr && fr->d) ? fr_create(fr->d, fr->n, fr->neg) : NULL;
}

frac *fr_add(frac *a, frac *b) {
    if (!a && !b) return NULL;
    if (!a) return fr_create(b->n, b->d, b->neg);
    if (!b) return fr_create(a->n, a->d, a->neg);
    
    if (a->neg != b->neg) {
        frac *bb = fr_neg(b);
        frac *result = fr_sub(a, bb);
        fr_free(bb);
        return result;
    }
    
    num *d = nm_mult(a->d, b->d);
    num *aa = nm_mult(a->n, b->d);
    num *bb = nm_mult(b->n, a->d);
    num *s = nm_add(aa, bb);
    nm_free(aa);
    nm_free(bb);
    frac *result = nm_create(s, d, a->neg);
    nm_free(d);
    nm_free(s);
    return result;
}

frac *fr_sub(frac *a, frac *b) {
    if (!a && !b) return NULL;
    if (!a) return fr_neg(b);
    if (!b) return fr_create(a->n, a->d, a->neg);
    
    if (a->neg != b->neg) {
        frac *bb = fr_neg(b);
        frac *result = fr_add(a, bb);
        fr_free(bb);
        return result;
    }
    
    if (!fr_leq(b, a)) {
        frac *rr = fr_sub(b, a);
        frac *result = fr_neg(rr);
        fr_free(rr);
        return result;
    }
    
    num *d = nm_mult(a->d, b->d);
    num *aa = nm_mult(a->n, b->d);
    num *bb = nm_mult(b->n, a->d);
    num *s = nm_sub(aa, bb);
    nm_free(aa);
    nm_free(bb);
    frac *result = nm_create(s, d, a->neg);
    nm_free(d);
    nm_free(s);
    return result;
}

frac *fr_mult(frac *a, frac *b) {
    if (!a || !b) return NULL;
    
    num *n = nm_mult(a->n, b->n);
    num *d = nm_mult(a->d, b->d);
    bool neg = a->neg != b->neg;
    frac *result = nm_create(n, d, neg);
    nm_free(n);
    nm_free(d);
    return result;
}

frac *fr_div(frac *a, frac *b) {
    if (!a || !b) return NULL;
    
    num *n = nm_mult(a->n, b->d);
    num *d = nm_mult(a->d, b->n);
    bool neg = a->neg != b->neg;
    frac *result = nm_create(n, d, neg);
    nm_free(n);
    nm_free(d);
    return result;
}

    bool fr_eq(frac *a, frac *b);
    bool fr_leq(frac *a, frac *b);
