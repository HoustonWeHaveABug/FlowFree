CONVERT_THOMASAHLE_C_FLAGS=-c -O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings

convert_thomasahle: convert_thomasahle.o
	gcc -o convert_thomasahle convert_thomasahle.o

convert_thomasahle.o: convert_thomasahle.c convert_thomasahle.make
	gcc ${CONVERT_THOMASAHLE_C_FLAGS} -o convert_thomasahle.o convert_thomasahle.c

clean:
	rm -f convert_thomasahle convert_thomasahle.o
