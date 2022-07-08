CONVERT_THOMASAHLE_C_FLAGS=-g -c -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

convert_thomasahle_debug: convert_thomasahle_debug.o
	gcc -g -o convert_thomasahle_debug convert_thomasahle_debug.o

convert_thomasahle_debug.o: convert_thomasahle.c convert_thomasahle_debug.make
	gcc ${CONVERT_THOMASAHLE_C_FLAGS} -o convert_thomasahle_debug.o convert_thomasahle.c

clean:
	rm -f convert_thomasahle_debug convert_thomasahle_debug.o
