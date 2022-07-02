#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned long x;
	unsigned long y;
}
coord_t;

typedef struct {
	int symbol;
	coord_t start;
	coord_t end;
}
color_t;

int main(void) {
	unsigned long columns_n, rows_n, colors_n, i;
	color_t *colors;
	if (scanf("%lu%lu", &columns_n, &rows_n) != 2 || rows_n < 1UL || columns_n < 1UL || rows_n*columns_n < 2UL) {
		fprintf(stderr, "Invalid parameters\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	getchar();
	colors = NULL;
	colors_n = 0UL;
	for (i = 0UL; i < rows_n; i++) {
		unsigned long j;
		for (j = 0UL; j < columns_n; j++) {
			int c = getchar();
			if (c != '.') {
				unsigned long number;
				for (number = 0UL; number < colors_n && colors[number].symbol != c; number++);
				if (number < colors_n) {
					colors[number].end.x = j;
					colors[number].end.y = i;
				}
				else {
					if (colors_n > 0UL) {
						color_t *colors_tmp = realloc(colors, sizeof(color_t)*(number+1UL));
						if (!colors_tmp) {
							fprintf(stderr, "Could not reallocate memory for colors\n");
							fflush(stderr);
							free(colors);
							return EXIT_FAILURE;
						}
						colors = colors_tmp;
					}
					else {
						colors = malloc(sizeof(color_t));
						if (!colors) {
							fprintf(stderr, "Could not allocate memory for colors\n");
							fflush(stderr);
							return EXIT_FAILURE;
						}
					}
					colors_n++;
					colors[number].symbol = c;
					colors[number].start.x = j;
					colors[number].start.y = i;
					colors[number].end.x = columns_n;
					colors[number].end.y = rows_n;
				}
			}
		}
		getchar();
	}
	printf("%lu %lu %lu\n", colors_n, columns_n, rows_n);
	for (i = 0UL; i < colors_n; i++) {
		printf("(%lu, %lu) ", colors[i].start.x, colors[i].start.y);
		printf("(%lu, %lu)\n", colors[i].end.x, colors[i].end.y);
	}
	fflush(stdout);
	if (colors_n > 0UL) {
		free(colors);
	}
	return EXIT_SUCCESS;
}
