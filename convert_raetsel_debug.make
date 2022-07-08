CONVERT_RAETSEL_C_FLAGS=-g -c -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

convert_raetsel_debug: convert_raetsel_debug.o
	gcc -g -o convert_raetsel_debug convert_raetsel_debug.o

convert_raetsel_debug.o: convert_raetsel.c convert_raetsel_debug.make
	gcc ${CONVERT_RAETSEL_C_FLAGS} -o convert_raetsel_debug.o convert_raetsel.c

clean:
	rm -f convert_raetsel_debug convert_raetsel_debug.o
