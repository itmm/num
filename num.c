#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "num.h"

static const unsigned mod = 1000000000;
#define cnt 8

struct num {
    num *next;
    unsigned vals[cnt];
};


#pragma mark - output numbers

static bool _nm_put_entry(FILE *out, num *nm) {
    if (!nm) return false;
    bool something = _nm_put_entry(out, nm->next);
    int c = cnt - 1;
    if (!something) {
        nm->next = nm_free(nm->next);
        while (c >= 0 && !nm->vals[c]) --c;
    }
    for (; c >= 0; --c) {
        fprintf(out, something ? "%09u" : "%u", nm->vals[c]);
        something = true;
    }
    return something;
}

void nm_put(FILE *out, num *nm) {
    if (!_nm_put_entry(out, nm)) fprintf(out, "0");
}


#pragma mark - memory handling

static num *_nm_alloc() {
    num *result = malloc(sizeof(num));
    if (result) {
        memset(result, 0, sizeof(num));
    } else {
        fprintf(stderr, "out of memory\n");
    }
    return result;
}

num *nm_free(num *nm) {
    while (nm) {
        num *next = nm->next;
        free(nm);
        nm = next;
    }
    return NULL;
}

num *nm_create(unsigned long v) {
    if (!v) return NULL;
    num *first = NULL;
    num *cur = NULL;
    int c = cnt - 1;
    for (; v; v /= mod) {
        if (++c == cnt) {
            num *nw = _nm_alloc();
            if (!nw) { return nm_free(first); }
            if (cur) { cur->next = nw; } else { first = nw; }
            cur = nw;
            c = 0;
        }
        cur->vals[c] = v % mod;
    }
    return first;
}

num *nm_parse(const char *value) {
    if (!value) return NULL;
    
    const char *cur = value + strlen(value) - 1;
    while (*value == '0') ++value;

    num *first = NULL;
    num *last = NULL;
    int c = cnt - 1;
    while (cur >= value) {
        unsigned v = 0;
        for (int m = 1; cur >= value && m < mod; m *= 10) {
            v += m * (*cur-- - '0');
        }
        if (++c == cnt) {
            num *nw = _nm_alloc();
            if (!nw) { return nm_free(first); }
            if (last) { last->next = nw; } else { first = nw; }
            last = nw;
            c = 0;
        }
        last->vals[c] = v;
    }
    return first;
}

#pragma mark - adding two numbers inplace

num *nm_add(num *res, num *other) {
    if (!other) { return res; }
    
    num *cur = res;
    num *lst = NULL;;
    unsigned mult = 0;
    while (mult || other) {
        if (!cur) {
            cur = _nm_alloc();
            if (!cur) { return nm_free(res); }
            if (lst) { lst->next = cur; } else { res = cur; }
        }
        for (int i = 0; i < cnt; ++i) {
            mult += cur->vals[i];
            if (other) mult += other->vals[i];
            cur->vals[i] = mult % mod;
            mult /= mod;
        }
        if (other) other = other->next;
        lst = cur;
        cur = cur->next;
    }
    
    return res;
}


#pragma mark - subtracting two numbers inplace

static num *_nm_normalize(num *nm) {
    if (!nm) { return NULL; }
    nm->next = _nm_normalize(nm->next);
    if (nm->next) return nm;
    for (int i = 0; i < cnt; ++i) {
        if (nm->vals[i]) return nm;
    }
    return nm_free(nm);
}

num *nm_sub(num *res, num *other) {
    if (!other) { return NULL; }
    unsigned carry = 0;
    
    num *cur = res;
    while (carry || other) {
        if (!cur) {
            printf("underflow\n");
            return nm_free(res);
        }
        for (int i = 0; i < cnt; ++i) {
            if (other) carry += other->vals[i];
            if (carry <= cur->vals[i]) {
                cur->vals[i] -= carry;
                carry = 0;
            } else {
                cur->vals[i] = (cur->vals[i] + mod - carry) % mod;
                carry = 1;
            }
        }
        if (other) other = other->next;
        cur = cur->next;
    }
    
    return _nm_normalize(res);
}


#pragma mark - multiplication (not inplace)

static num *_nm_shift(num *nm) {
    if (!nm) return NULL;
    unsigned v = 0;
    num *cur = nm;
    while (cur) {
        for (int i = 0; i < cnt; ++i) {
            unsigned t = cur->vals[i]; cur->vals[i] = v; v = t;
        }
        if (!cur->next && v) {
            cur->next = _nm_alloc();
            if (!cur->next) { return nm_free(nm); }
        }
        cur = cur->next;
    }
    return nm;
}

static num *_nm_unshift(num *nm) {
    if (!nm) return NULL;
    num *cur = nm;
    while (cur) {
        for (int i = 1; i < cnt; ++i) {
            cur->vals[i - 1] = cur->vals[i];
        }
        cur->vals[cnt - 1] = cur->next ? cur->next->vals[0] : 0;
        cur = cur->next;
    }
    return _nm_normalize(nm);
}

static num *_nm_mult_one(num *res, num *a, unsigned  b) {
    if (!a || !b) { return nm_free(res); }
    
    unsigned long mult = 0;
    num *cur = res;
    num *last = NULL;
    while (a || mult) {
        if (!cur) {
            cur = _nm_alloc();
            if (!cur) { return nm_free(res); }
            if (last) { last->next = cur; } else { res = cur; }
        }
        for (int i = 0; i < cnt; ++i) {
            if (a) { mult += a->vals[i] * (unsigned long) b; }
            cur->vals[i] = mult % mod;
            mult /= mod;
        }
        if (a) { a = a->next; }
        last = cur;
        cur = cur->next;
    }
    if (cur) {
        nm_free(cur);
        if (last) { last->next = NULL; } else { res = NULL; }
    }
    return res;
}


num *nm_mult(num *a, num *b) {
    if (!a || !b) return NULL;
    num *res = NULL;
    num *shifted = nm_add(NULL, a);
    num *tmp = NULL;
    for (; b; b = b->next) {
        for (int i = 0; i < cnt; ++i) {
            if (b->vals[i]) {
                tmp = _nm_mult_one(tmp, shifted, b->vals[i]);
                res = nm_add(res, tmp);
            }
            shifted = _nm_shift(shifted);
        }
    }
    nm_free(tmp);
    nm_free(shifted);
    return res;
}


#pragma mark - division with rest (not inplace)

unsigned _nm_factor1(num *a, num *b) {
    if (!a || !b) return 0;
    
    unsigned a1 = 0, a2 = 0;
    bool some_a2 = false;
    
    for (; a->next && b->next; a = a->next, b = b->next);
    
    unsigned factor = 0;
    if (!a->next && !b->next) {
        int i = cnt; while (i && !a->vals[i - 1]) --i;
        int j = cnt; while (j && !b->vals[j - 1]) --j;
        if (i && j) {
            factor = a->vals[i - 1]/b->vals[j - 1];
        }
    }
    return factor;
}

unsigned _nm_factor2(num *a, num *b) {
    if (!a || !b) return 0;
    
    unsigned a2 = 0;
    bool some_a2 = false;
    
    for (; a->next && b->next; a = a->next, b = b->next) {
        some_a2 = true;
        a2 = a->vals[cnt - 1];
    }
    unsigned factor = 0;
    if (!a->next && !b->next) {
        int i = cnt; while (i && !a->vals[i - 1]) --i;
        int j = cnt; while (j && !b->vals[j - 1]) --j;
        if (i && j) {
            if (i > 1) { some_a2 = true; a2 = a->vals[i - 2]; }
            if (some_a2) {
                unsigned long aa = ((unsigned long) a->vals[i - 1]) * mod + a2;
                unsigned long bb = b->vals[j - 1];
                factor = aa/bb;
            }
        }
    }
    return factor;
}

unsigned _nm_factor(num *a, num *b) {
    unsigned res = _nm_factor1(a, b);
    if (res) return res;
    return _nm_factor2(a, b);
}

num *nm_div(num *a, num *b, num **modulus) {
    if (!b) { printf("division by 0\n"); return NULL; }
    if (!a) { return NULL; }
    
    num *rest = nm_add(NULL, a);
    num *res = NULL;
    
    if (nm_leq(b, rest)) {
        num *rest_shifted = nm_add(NULL, b);
        num *res_shifted = nm_create(1);
        num *tmp = NULL;
        while (nm_leq(rest_shifted, rest)) {
            rest_shifted = _nm_shift(rest_shifted);
            res_shifted = _nm_shift(res_shifted);
        }
        while (nm_leq(b, rest)) {
            while (!nm_leq(rest_shifted, rest)) {
                rest_shifted = _nm_unshift(rest_shifted);
                res_shifted = _nm_unshift(res_shifted);
            }
            unsigned factor = _nm_factor(rest, rest_shifted);
            if (!factor) {
                printf("factor error\n");
                nm_free(res_shifted);
                nm_free(rest_shifted);
                nm_free(tmp);
                nm_free(res);
                nm_free(rest);
                return NULL;
            }
            tmp = _nm_mult_one(tmp, rest_shifted, factor);
            rest = nm_sub(rest, tmp);
            tmp = _nm_mult_one(tmp, res_shifted, factor);
            res = nm_add(res, tmp);
        }
        nm_free(tmp);
        nm_free(res_shifted);
        nm_free(rest_shifted);
    }
    
    if (modulus) { *modulus = rest; } else { nm_free(rest); }
    return res;
}


#pragma mark - comparison

bool nm_eq(num *a, num *b) {
    for (; a && b; a = a->next, b = b->next) {
        for (int i = 0; i < cnt; ++i) {
            if (a->vals[i] != b->vals[i]) return false;
        }
    }
    return !a && !b;
}

bool nm_leq(num *a, num *b) {
    bool isLeq = true;
    for (; a && b; a = a->next, b = b->next) {
        for (int i = 0; i < cnt; ++i) {
            isLeq = a->vals[i] < b->vals[i] || (isLeq && a->vals[i] == b->vals[i]);
        }
    }
    if (b) return true;
    if (a) return false;
    return isLeq;
}

