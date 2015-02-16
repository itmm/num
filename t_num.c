#include "num.h"

#include <assert.h>
#include <string.h>

static void t_alloc_0() {
    assert(nm_create(0) == NULL);
}

static void t_dealloc_0() {
    assert(nm_free(NULL) == NULL);
}

static void assert_eq_f(num *v, const char *ref) {
    static char buffer[60];

    FILE *f = tmpfile();
    nm_put(f, v);
    fseek(f, 0, SEEK_SET);
    int readed = fread(buffer, 1, sizeof(buffer), f);
    assert(readed == strlen(ref));
    assert(memcmp(ref, buffer, readed) == 0);
    fclose(f);
}

static void t_put_0() {
    assert_eq_f(NULL, "0");
}

static void t_put() {
    num *v = nm_create(42);
    assert_eq_f(v, "42");
    nm_free(v);
}

static void assert_eq_nm(num *v, const char *ref) {
    num *o = nm_parse(ref);
    assert(nm_eq(v, o));
    nm_free(o);
}

static void t_equal_0() {
    assert_eq_nm(NULL, "0");
}

static void t_equal_000() {
    assert(nm_parse("000") == NULL);
}

static void t_equal_NULL() {
    assert(nm_parse(NULL) == NULL);
}

static void t_equal_42() {
    num *v = nm_create(42);
    assert_eq_nm(v, "42");
    nm_free(v);
}

static void t_equal_big() {
    const char *big = "124567892245678932456789424567895245678962456789724567898245678992456789";
    num *v = nm_parse(big);
    assert_eq_nm(v, big);
    nm_free(v);
}

static void t_add_0_0() {
    num *r = nm_add(NULL, NULL);
    assert(r == NULL);
}

static void t_add_100_0() {
    num *a = nm_create(100);
    num *r = nm_add(a, NULL);
    assert_eq_nm(r, "100");
    nm_free(r);
    nm_free(a);
}

static void t_add_0_23() {
    num *b = nm_create(23);
    num *r = nm_add(NULL, b);
    assert_eq_nm(r, "23");
    nm_free(r);
    nm_free(b);
}

static void t_add_100_23() {
    num *a = nm_create(100);
    num *b = nm_create(23);
    num *r = nm_add(a, b);
    assert_eq_nm(r, "123");
    nm_free(r);
    nm_free(b);
    nm_free(a);
}

static void t_add_overflow() {
    num *a = nm_create(1234567890);
    num *b = nm_create(2345678901);
    num *r = nm_add(a, b);
    assert_eq_nm(r, "3580246791");
    nm_free(r);
    nm_free(b);
    nm_free(a);
}

static void t_add_self() {
    num *v = nm_create(987654321);
    num *r = nm_add(v, v);
    assert_eq_nm(r, "1975308642");
    nm_free(r);
    nm_free(v);
}

static void t_add_big() {
    num *v = nm_parse("800000000700000000600000000500000000400000000300000000200000000100000000");
    num *r = nm_add(v, v);
    assert_eq_nm(r, "1600000001400000001200000001000000000800000000600000000400000000200000000");
    nm_free(r);
    nm_free(v);
}

static void t_sub_small() {
    num *a = nm_create(234);
    num *b = nm_create(34);
    num *r = nm_sub(a, b);
    assert_eq_nm(r, "200");
    nm_free(r);
    nm_free(b);
    nm_free(a);
}

static void t_sub_carry() {
    num *a = nm_create(1000000000);
    num *b = nm_create(100);
    num *r = nm_sub(a, b);
    assert_eq_nm(r, "999999900");
    nm_free(r);
    nm_free(b);
    nm_free(a);
}

static void t_sub_big() {
    num *v = nm_parse("1800000000700000000600000000500000000400000000300000000200000000100000000");
    num *r = nm_sub(v, v);
    assert(r == NULL);
    nm_free(v);
}

static void t_mult_simple() {
    num *a = nm_create(123456789);
    num *b = nm_create(100000000);
    num *r = nm_mult(a, b);
    assert_eq_nm(r, "12345678900000000");
    nm_free(r);
    nm_free(b);
    nm_free(a);
}

static void t_div_simple() {
    num *a = nm_parse("123456789123456789");
    num *b = nm_create(1000);
    num *m = NULL;
    num *r = nm_div(a, b, &m);
    assert_eq_nm(r, "123456789123456");
    assert_eq_nm(m, "789");
    nm_free(r);
    nm_free(m);
    nm_free(b);
    nm_free(a);
}

static int tests = 0;

#define T(t) { ++tests; putchar('.'); t(); }

int main() {
    T(t_alloc_0);
    T(t_dealloc_0);
    T(t_put);
    T(t_put_0);
    T(t_equal_0);
    T(t_equal_000)
    T(t_equal_NULL);
    T(t_equal_42);
    T(t_equal_big);
    T(t_add_0_0);
    T(t_add_100_0);
    T(t_add_0_23);
    T(t_add_100_23);
    T(t_add_overflow);
    T(t_add_self);
    T(t_add_big);
    T(t_sub_small);
    T(t_sub_carry);
    T(t_sub_big);
    T(t_mult_simple);
    T(t_div_simple);
    printf("\n\ntests: %d\n", tests);
}
