CFLAGS += -pedantic -O3

all: tests

clean:
	rm *.o t_num

tests: t_num t_frac
	./t_num
	./t_frac

t_num: t_num.o num.o
t_frac: t_frac.o frac.o num.o

t_num.c: num.h
num.c: num.h

t_frac.c: frac.h
frac.h: num.h
frac.c: frac.h
