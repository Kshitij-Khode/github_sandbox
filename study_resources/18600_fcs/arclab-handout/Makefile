####################################################
# Students' Makefile for the 18-600 Architecture Lab
####################################################

CC = gcc
CFLAGS = -Wall -Werror -Wno-unused-parameter -O0
MMOBJ = mm.o
DAXOBJ = daxpy.o


mm: $(MMOBJ)
	$(CC) $(CFLAGS) -o mm $(MMOBJ)

daxpy: $(DAXOBJ)
	$(CC) $(CFLAGS) -o daxpy $(DAXOBJ)

sim:
	(cd sim; make)

# Use this rule to hand in the lab ("make handin")
handin:
	rm -rf arclab-handin
	rm -rf arclab-handin.tar
	mkdir arclab-handin
	cp -p sim/misc/sum.ys arclab-handin
	cp -p sim/misc/rsum.ys arclab-handin
	cp -p sim/misc/copy.ys arclab-handin
	cp -p sim/pipe/ncopy.ys arclab-handin
	cp -p mm.c arclab-handin
	cp -p daxpy.c arclab-handin
	cp -p o3_mm.py arclab-handin
	cp -p o3_daxpy.py arclab-handin
	cp -p o3_star.py arclab-handin
	tar -czvf arclab-handin.tar arclab-handin

clean:
	rm -rf *~ *.o arclab-handin arclab-handin.tar mm daxpy

rmgem:
	rm -rf out
	rm -rf m5out

*.o: *.c
