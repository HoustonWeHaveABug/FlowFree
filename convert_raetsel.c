#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned long x;
	unsigned long y;
}
coord_t;

typedef struct {
	coord_t start;
	coord_t end;
}
color_t;

void init_colors(unsigned long, unsigned long);

unsigned long rows_n, columns_n;
color_t *colors;

int main(void) {
	unsigned long colors_lo, colors_hi, i;
	if (scanf("%lu%lu", &rows_n, &columns_n) != 2 || rows_n < 1UL || columns_n < 1UL || rows_n*columns_n < 2UL) {
		fprintf(stderr, "Invalid parameters\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	colors = malloc(sizeof(color_t));
	if (!colors) {
		fprintf(stderr, "Could not allocate memory for colors\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	colors_lo = 0UL;
	colors_hi = 1UL;
	init_colors(colors_lo, colors_hi);
	for (i = 0UL; i < rows_n; i++) {
		unsigned long j;
		for (j = 0UL; j < columns_n; j++) {
			unsigned long number;
			if (scanf("%lu", &number) == 1) {
				if (number > colors_hi) {
					color_t *colors_tmp = realloc(colors, sizeof(color_t)*number);
					if (!colors_tmp) {
						fprintf(stderr, "Could not reallocate memory for colors\n");
						fflush(stderr);
						free(colors);
						return EXIT_FAILURE;
					}
					colors = colors_tmp;
					colors_lo = colors_hi;
					colors_hi = number;
					init_colors(colors_lo, colors_hi);
				}
				number--;
				if (colors[number].start.x < columns_n) {
					colors[number].end.x = j;
					colors[number].end.y = i;
				}
				else {
					colors[number].start.x = j;
					colors[number].start.y = i;
				}
			}
			else {
				getchar();
			}
		}
	}
	printf("%lu %lu %lu\n", colors_hi, columns_n, rows_n);
	for (i = 0UL; i < colors_hi; i++) {
		printf("(%lu, %lu) ", colors[i].start.x, colors[i].start.y);
		printf("(%lu, %lu)\n", colors[i].end.x, colors[i].end.y);
	}
	fflush(stdout);
	free(colors);
	return EXIT_SUCCESS;
}

void init_colors(unsigned long lo, unsigned long hi) {
	unsigned long i;
	for (i = lo; i < hi; i++) {
		colors[i].start.x = columns_n;
		colors[i].start.y = rows_n;
		colors[i].end.x = columns_n;
		colors[i].end.y = rows_n;
	}
}
