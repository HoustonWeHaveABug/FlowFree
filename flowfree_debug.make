FLOWFREE_DEBUG_C_FLAGS=-g -c -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

flowfree_debug: flowfree_debug.o
	gcc -g -o flowfree_debug flowfree_debug.o

flowfree_debug.o: flowfree.c flowfree_debug.make
	gcc ${FLOWFREE_DEBUG_C_FLAGS} -o flowfree_debug.o flowfree.c

clean:
	rm -f flowfree_debug flowfree_debug.o
