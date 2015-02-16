CFLAGS += -pedantic -O3

all: tests

clean:
	rm *.o t_num

tests: t_num
	./t_num

t_num: t_num.o num.o

t_num.c: num.h
num.c: num.h
