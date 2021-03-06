CONVERT_RAETSEL_C_FLAGS=-c -O2 -Wall -Wextra -Waggregate-return -Wcast-align -Wcast-qual -Wconversion -Wformat=2 -Winline -Wlong-long -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Wno-import -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wstrict-prototypes -Wswitch -Wwrite-strings

convert_raetsel.exe: convert_raetsel.o
	gcc -o convert_raetsel.exe convert_raetsel.o

convert_raetsel.o: convert_raetsel.c convert_raetsel.make
	gcc ${CONVERT_RAETSEL_C_FLAGS} -o convert_raetsel.o convert_raetsel.c
