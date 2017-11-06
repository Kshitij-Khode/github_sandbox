#
# Student makefile for Cache Lab
# 
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99

all: csim msim test-trans tracegen-ct
	-tar -cvf ${USER}_handin.tar  csim.c msim.c trans.c cache.c

csim: \
	csim-driver.c \
	csim.h csim.c \
	cache.c cache.h \
	trace-stream.h trace-stream.c 
	$(CC) $(CFLAGS) -o csim \
	  csim-driver.c csim.c cache.c trace-stream.c -lm

msim: \
	msim-driver.c \
	msim.h msim.c \
	cache.c cache.h \
	trace-stream.h trace-stream.c
	$(CC) $(CFLAGS) -o msim \
	  msim-driver.c msim.c cache.c trace-stream.c -lm

test-trans: test-trans.c trans.o cachelab.c cachelab.h
	$(CC) $(CFLAGS) -o test-trans test-trans.c cachelab.c trans.o 

tracegen-ct: tracegen-ct.c trans.c cachelab.c
	clang -emit-llvm -S -O3 trans.c -o trans.bc
	opt trans.bc -load=ct/Contech.so -Contech -o trans_ct.bc
	llvm-link trans_ct.bc ct/ct.bc -o trans_fin.bc
	clang -o tracegen-ct -O3 trans_fin.bc cachelab.c tracegen-ct.c -pthread -lrt

trans.o: trans.c
	$(CC) $(CFLAGS) -O0 -c trans.c

#
# Clean the src directory
#
clean:
	rm -rf *.o
	rm -f *.bc
	rm -f csim
	rm -f msim
	rm -f test-trans tracegen tracegen-ct
	rm -f trace.all trace.f*
	rm -f .csim_results .marker
