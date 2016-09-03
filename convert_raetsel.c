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

int main(void) {
unsigned long colors_n, rows_n, columns_n, i, j, number;
color_t *colors;
	if (scanf("%lu%lu%lu", &colors_n, &rows_n, &columns_n) != 3 || !colors_n || rows_n < 2 || columns_n < 2) {
		fprintf(stderr, "Invalid parameters\n");
		return EXIT_FAILURE;
	}
	colors = malloc(sizeof(color_t)*colors_n);
	if (!colors) {
		fprintf(stderr, "Could not allocate memory for colors\n");
		return EXIT_FAILURE;
	}
	for (i = 0; i < colors_n; i++) {
		colors[i].start.x = columns_n;
		colors[i].start.y = rows_n;
		colors[i].end.x = columns_n;
		colors[i].end.y = rows_n;
	}
	for (i = 0; i < rows_n; i++) {
		for (j = 0; j < columns_n; j++) {
			if (scanf("%lu", &number) == 1) {
				if (colors[number-1].start.x < columns_n) {
					colors[number-1].end.x = j;
					colors[number-1].end.y = i;
				}
				else {
					colors[number-1].start.x = j;
					colors[number-1].start.y = i;
				}
			}
			else {
				fgetc(stdin);
			}
		}
	}
	printf("%lu %lu %lu\n", colors_n, columns_n, rows_n);
	for (i = 0; i < colors_n; i++) {
		printf("(%lu, %lu) ", colors[i].start.x, colors[i].start.y);
		printf("(%lu, %lu)\n", colors[i].end.x, colors[i].end.y);
	}
	free(colors);
	return EXIT_SUCCESS;
}
