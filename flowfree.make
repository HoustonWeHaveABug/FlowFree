FLOWFREE_C_FLAGS=-c -O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

flowfree: flowfree.o
	gcc -o flowfree flowfree.o

flowfree.o: flowfree.c flowfree.make
	gcc ${FLOWFREE_C_FLAGS} -o flowfree.o flowfree.c

clean:
	rm -f flowfree flowfree.o
