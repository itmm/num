#include "frac.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void assert_eq_f(frac *v, const char *ref) {
    static char buffer[60];

    FILE *f = tmpfile();
    fr_put(f, v);
    fseek(f, 0, SEEK_SET);
    int readed = fread(buffer, 1, sizeof(buffer), f);
    assert(readed == strlen(ref));
    assert(memcmp(ref, buffer, readed) == 0);
    fclose(f);
}


void t_null() {
    assert_eq_f(NULL, "0");
}

void t_integer() {
    frac *v = fr_create_simple(300, 10);
    assert_eq_f(v, "30");
    fr_free(v);
}

void t_simplify() {
    frac *v = fr_create_simple(30, 70);
    assert_eq_f(v, "3/7");
    fr_free(v);
}

void t_out_neg() {
    frac *v = fr_create_simple(-3, 8);
    assert_eq_f(v, "-3/8");
    fr_free(v);
}

void t_add_simple() {
    frac *a = fr_create_simple(2, 3);
    frac *b = fr_create_simple(5, 7);
    frac *r = fr_add(a, b);
    assert_eq_f(r, "29/21");
    fr_free(r);
    fr_free(b);
    fr_free(a);
}

void t_div_simple() {
    frac *a = fr_create_simple(2, 3);
    frac *b = fr_create_simple(5, 7);
    frac *r = fr_div(a, b);
    assert_eq_f(r, "14/15");
    fr_free(r);
    fr_free(b);
    fr_free(a);
}

static int tests = 0;

#define T(t) { ++tests; putchar('.'); t(); }

int main() {
    T(t_null);
    T(t_integer);
    T(t_simplify);
    T(t_out_neg);
    T(t_add_simple);
    T(t_div_simple);
    printf("\n\ntests: %d\n", tests);
}
