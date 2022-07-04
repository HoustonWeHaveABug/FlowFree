#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define LINKS_MAX 2UL
#define OPTIONS_MAX 4UL
#define CHOICES_MAX 3UL
#define PIPE_W 1UL
#define PIPE_N 2UL
#define PIPE_S 4UL
#define PIPE_E 8UL
#define CALLS_MAX 12UL
#define ROLLS_MAX 4UL

typedef struct cell_s cell_t;
typedef struct color_s color_t;

struct cell_s {
	unsigned long col;
	unsigned long row;
	unsigned long pipes;
	cell_t *path;
	unsigned long rank;
	color_t *color;
	unsigned long links_todo;
	unsigned long links_done;
	cell_t *links[LINKS_MAX];
	cell_t *last;
	cell_t *next;
};

struct color_s {
	cell_t *start;
	cell_t *end;
};

typedef enum {
	CALL_SELECTION,
	CALL_CHAIN,
	CALL_UNROLL,
	CALL_ROLL,
	CALL_UNCHAIN
}
call_t;

typedef struct {
	unsigned long pipe_used;
	cell_t *link;
}
option_t;

typedef struct {
	cell_t *cell;
}
selection_t;

typedef struct {
	cell_t *cell;
	unsigned long options_n;
	option_t options[OPTIONS_MAX];
}
chain_t;

typedef struct {
	cell_t *cell;
	cell_t *link;
	cell_t *cell_path;
	color_t *cell_color;
	cell_t *link_path;
	color_t *link_color;
}
unroll_t;

typedef struct {
	cell_t *cell;
	cell_t *link;
	unroll_t *unroll;
}
roll_t;

typedef struct {
	cell_t *link;
	unroll_t *unroll;
}
choice_t;

typedef struct {
	unsigned long options_n;
	option_t options[OPTIONS_MAX];
	unsigned long choices_n;
	choice_t choices[CHOICES_MAX][LINKS_MAX];
}
constraint_t;

void set_cell(cell_t *, unsigned long, unsigned long, unsigned long);
void link_cell(cell_t *, cell_t *, cell_t *);
int read_color(color_t *);
cell_t *read_cell(int);
void set_color(color_t *);
unsigned long random_xy(unsigned long *, unsigned long *);
unsigned long erand(unsigned long);
unsigned long get_distance(unsigned long, unsigned long, unsigned long, unsigned long);
void set_cell_color(cell_t *, color_t *);
void flowfree(void);
void stack_call(call_t *, call_t);
void perform_call(call_t *);
void stack_selection(selection_t *, cell_t *);
void perform_selection(selection_t *);
void set_constraint(cell_t *, unsigned long, constraint_t *);
unsigned long set_options_empty(cell_t *, option_t []);
unsigned long set_options_w(cell_t *, option_t []);
unsigned long set_options_n(cell_t *, option_t []);
unsigned long set_options_wn(cell_t *, option_t []);
unsigned long set_options_s(cell_t *, option_t []);
unsigned long set_options_ws(cell_t *, option_t []);
unsigned long set_options_ns(cell_t *, option_t []);
unsigned long set_options_wns(cell_t *, option_t []);
unsigned long set_options_e(cell_t *, option_t []);
unsigned long set_options_we(cell_t *, option_t []);
unsigned long set_options_ne(cell_t *, option_t []);
unsigned long set_options_wne(cell_t *, option_t []);
unsigned long set_options_se(cell_t *, option_t []);
unsigned long set_options_wse(cell_t *, option_t []);
unsigned long set_options_nse(cell_t *, option_t []);
unsigned long set_options_wnse(cell_t *, option_t []);
void set_option(option_t *, unsigned long, cell_t *);
void add_roll_option(cell_t *, option_t *);
void add_choices(cell_t *, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long *, choice_t [][LINKS_MAX]);
int roll_option(cell_t *, cell_t *);
int no_touching(cell_t *);
int touching_w(cell_t *);
int touching_n(cell_t *);
int touching_wn(cell_t *);
int touching_s(cell_t *);
int touching_ws(cell_t *);
int touching_ns(cell_t *);
int touching_e(cell_t *);
int touching_we(cell_t *);
int touching_ne(cell_t *);
int touching_se(cell_t *);
int same_path(cell_t *, cell_t *);
void stack_chain(chain_t *, cell_t *, unsigned long, option_t []);
void stack_unroll(unroll_t *, cell_t *, choice_t *);
void stack_roll(roll_t *, cell_t *, choice_t *);
void stack_unchain(chain_t *, cell_t *, unsigned long, option_t []);
void print_color(color_t *);
void print_cell(cell_t *, int);
void perform_chain(chain_t *);
void add_pipe(cell_t *, unsigned long, cell_t *);
void chain_cell(cell_t *);
void perform_unroll(unroll_t *);
void unroll_link(cell_t *, cell_t *, cell_t *, color_t *, cell_t *, color_t *);
void perform_roll(roll_t *);
void roll_link(cell_t *, cell_t *, unroll_t *);
void perform_unchain(chain_t *);
void unchain_cell(cell_t *);
void remove_pipe(cell_t *, unsigned long, cell_t *);
void update_path(cell_t *, cell_t *);
cell_t *next_cell(cell_t *, cell_t *);
void reset_color(color_t *);

int grid_rotated, touching_allowed, (*touching[])(cell_t *) = { no_touching, touching_w, touching_n, touching_wn, touching_s, touching_ws, touching_ns, no_touching, touching_e, touching_we, touching_ne, no_touching, touching_se, no_touching, no_touching, no_touching };
unsigned long colors_n, cols_n, rows_n, distance_min, solutions_max, nodes_n, solutions_n, stack_calls_n, stack_selections_n, stack_chains_n, stack_unrolls_n, stack_rolls_n, stack_unchains_n, (*set_options[])(cell_t *, option_t []) = { set_options_empty, set_options_w, set_options_n, set_options_wn, set_options_s, set_options_ws, set_options_ns, set_options_wns, set_options_e, set_options_we, set_options_ne, set_options_wne, set_options_se, set_options_wse, set_options_nse, set_options_wnse }, rolls_n, rolls_idx[LINKS_MAX];
cell_t *cells, *cells_header;
color_t *colors;
call_t *stack_calls;
option_t *rolls[OPTIONS_MAX];
selection_t *stack_selections;
chain_t *stack_chains, *stack_unchains;
unroll_t *stack_unrolls;
roll_t *stack_rolls;

int main(void) {
	unsigned long cells_n, row, col, attempts_n_mod;
	cell_t *cell;
	if (scanf("%lu%lu%lu", &colors_n, &cols_n, &rows_n) != 3 || colors_n < 1UL || cols_n < 1UL || rows_n < 1UL) {
		fprintf(stderr, "Invalid parameters\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	grid_rotated = cols_n < rows_n;
	if (grid_rotated) {
		unsigned long tmp = cols_n;
		cols_n = rows_n;
		rows_n = tmp;
	}
	cells_n = cols_n*rows_n;
	if (cells_n < 2UL || cells_n < colors_n*2UL) {
		fprintf(stderr, "Invalid parameters\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	cells = malloc(sizeof(cell_t)*(cells_n+1UL));
	if (!cells) {
		fprintf(stderr, "Could not allocate memory for cells\n");
		fflush(stderr);
		return EXIT_FAILURE;
	}
	if (cols_n == 1UL) {
		set_cell(cells, 0UL, 0UL, PIPE_S);
		cell = cells+1UL;
		for (row = 1UL; row < rows_n-1UL; row++) {
			set_cell(cell++, 0UL, row, PIPE_N+PIPE_S);
		}
		set_cell(cell++, 0UL, row, PIPE_N);
	}
	else if (rows_n == 1UL) {
		set_cell(cells, 0UL, 0UL, PIPE_E);
		cell = cells+1UL;
		for (col = 1UL; col < cols_n-1UL; col++) {
			set_cell(cell++, col, 0UL, PIPE_W+PIPE_E);
		}
		set_cell(cell++, col, 0UL, PIPE_W);
	}
	else {
		cell = cells;
		set_cell(cell++, 0UL, 0UL, PIPE_E+PIPE_S);
		for (row = 1UL; row < rows_n-1UL; row++) {
			set_cell(cell++, 0UL, row, PIPE_N+PIPE_E+PIPE_S);
		}
		set_cell(cell++, 0UL, row, PIPE_N+PIPE_E);
		for (col = 1UL; col < cols_n-1UL; col++) {
			set_cell(cell++, col, 0UL, PIPE_W+PIPE_E+PIPE_S);
			for (row = 1UL; row < rows_n-1UL; row++) {
				set_cell(cell++, col, row, PIPE_W+PIPE_N+PIPE_E+PIPE_S);
			}
			set_cell(cell++, col, row, PIPE_W+PIPE_N+PIPE_E);
		}
		set_cell(cell++, col, 0UL, PIPE_W+PIPE_S);
		for (row = 1UL; row < rows_n-1UL; row++) {
			set_cell(cell++, col, row, PIPE_W+PIPE_N+PIPE_S);
		}
		set_cell(cell++, col, row, PIPE_W+PIPE_N);
	}
	cells_header = cell;
	link_cell(cells, cells_header, cells+1UL);
	for (cell = cells+1UL; cell < cells_header; cell++) {
		link_cell(cell, cell-1UL, cell+1UL);
	}
	link_cell(cell, cell-1UL, cells);
	colors = malloc(sizeof(color_t)*colors_n);
	if (!colors) {
		fprintf(stderr, "Could not allocate memory for colors\n");
		fflush(stderr);
		free(cells);
		return EXIT_FAILURE;
	}
	if (scanf("%d", &touching_allowed) == 1) {
		if (scanf("%lu", &distance_min) != 1 || distance_min < 1UL || distance_min > cols_n+rows_n-colors_n-1UL || scanf("%lu%lu", &solutions_max, &attempts_n_mod) != 2 || solutions_max < 1UL || attempts_n_mod < 1UL) {
			fprintf(stderr, "Invalid generator parameters\n");
			fflush(stderr);
			return EXIT_FAILURE;
		}
	}
	else {
		unsigned long i;
		distance_min = 0UL;
		solutions_max = ULONG_MAX;
		attempts_n_mod = 1UL;
		for (i = 0UL; i < colors_n; i++) {
			if (!read_color(colors+i)) {
				free(colors);
				free(cells);
				return EXIT_FAILURE;
			}
		}
	}
	stack_calls = malloc(sizeof(call_t)*(cells_n*CALLS_MAX+1UL));
	if (!stack_calls) {
		fprintf(stderr, "Could not allocate memory for calls stack\n");
		fflush(stderr);
		free(colors);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_selections = malloc(sizeof(selection_t)*(cells_n*(CHOICES_MAX-1UL)+1UL));
	if (!stack_selections) {
		fprintf(stderr, "Could not allocate memory for selections stack\n");
		fflush(stderr);
		free(stack_calls);
		free(colors);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_chains = malloc(sizeof(chain_t)*cells_n);
	if (!stack_chains) {
		fprintf(stderr, "Could not allocate memory for chains stack\n");
		fflush(stderr);
		free(stack_selections);
		free(stack_calls);
		free(colors);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_unrolls = malloc(sizeof(unroll_t)*cells_n*ROLLS_MAX);
	if (!stack_unrolls) {
		fprintf(stderr, "Could not allocate memory for unrolls stack\n");
		fflush(stderr);
		free(stack_chains);
		free(stack_selections);
		free(stack_calls);
		free(colors);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_rolls = malloc(sizeof(roll_t)*cells_n*ROLLS_MAX);
	if (!stack_rolls) {
		fprintf(stderr, "Could not allocate memory for rolls stack\n");
		fflush(stderr);
		free(stack_unrolls);
		free(stack_chains);
		free(stack_selections);
		free(stack_calls);
		free(colors);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_unchains = malloc(sizeof(chain_t)*cells_n);
	if (!stack_unchains) {
		fprintf(stderr, "Could not allocate memory for unchains stack\n");
		fflush(stderr);
		free(stack_rolls);
		free(stack_unrolls);
		free(stack_chains);
		free(stack_selections);
		free(stack_calls);
		free(colors);
		free(cells);
		return EXIT_FAILURE;
	}
	if (distance_min > 0UL) {
		unsigned long attempts_n;
		srand((unsigned)time(NULL));
		attempts_n = 0UL;
		do {
			unsigned long i;
			for (i = 0UL; i < colors_n; i++) {
				set_color(colors+i);
			}
			flowfree();
			for (i = 0UL; i < colors_n; i++) {
				reset_color(colors+i);
			}
			attempts_n++;
			if (attempts_n%attempts_n_mod == 0UL) {
				printf("Attempts %lu\n", attempts_n);
				fflush(stdout);
			}
		}
		while (solutions_n == 0UL || solutions_n > solutions_max);
		printf("\nNodes %lu\nSolutions %lu\nAttempts %lu\n", nodes_n, solutions_n, attempts_n);
		fflush(stdout);
	}
	else {
		touching_allowed = 0;
		flowfree();
		printf("\nTouching forbidden\nNodes %lu\nSolutions %lu\n", nodes_n, solutions_n);
		fflush(stdout);
		if (solutions_n == 0UL) {
			touching_allowed = 1;
			flowfree();
			printf("\nTouching allowed\nNodes %lu\nSolutions %lu\n", nodes_n, solutions_n);
			fflush(stdout);
		}
	}
	free(stack_unchains);
	free(stack_rolls);
	free(stack_unrolls);
	free(stack_chains);
	free(stack_selections);
	free(stack_calls);
	free(colors);
	free(cells);
	return EXIT_SUCCESS;
}

void set_cell(cell_t *cell, unsigned long col, unsigned long row, unsigned long pipes) {
	unsigned long i;
	cell->col = col;
	cell->row = row;
	cell->pipes = pipes;
	cell->path = cell;
	cell->rank = 1UL;
	cell->color = NULL;
	cell->links_todo = LINKS_MAX;
	cell->links_done = 0UL;
	for (i = 0UL; i < LINKS_MAX; i++) {
		cell->links[i] = NULL;
	}
}

void link_cell(cell_t *cell, cell_t *last, cell_t *next) {
	cell->last = last;
	cell->next = next;
}

int read_color(color_t *color) {
	color->start = read_cell(' ');
	if (!color->start) {
		fprintf(stderr, "Invalid start cell\n");
		fflush(stderr);
		return 0;
	}
	if (color->start->color) {
		fprintf(stderr, "Start cell color already set\n");
		fflush(stderr);
		return 0;
	}
	color->end = read_cell('\n');
	if (!color->end) {
		fprintf(stderr, "Invalid end cell\n");
		fflush(stderr);
		return 0;
	}
	if (color->end->color) {
		fprintf(stderr, "End cell color already set\n");
		fflush(stderr);
		return 0;
	}
	set_cell_color(color->start, color);
	set_cell_color(color->end, color);
	return 1;
}

cell_t *read_cell(int c) {
	unsigned long x, y;
	if (grid_rotated) {
		if (scanf("(%lu, %lu)", &y, &x) != 2 || x > cols_n || y > rows_n || fgetc(stdin) != c) {
			return NULL;
		}
	}
	else {
		if (scanf("(%lu, %lu)", &x, &y) != 2 || x > cols_n || y > rows_n || fgetc(stdin) != c) {
			return NULL;
		}
	}
	return cells+rows_n*x+y;
}

void set_color(color_t *color) {
	unsigned long start_x, start_y, end_x, end_y;
	do {
		color->start = cells+random_xy(&start_x, &start_y);
		color->end = cells+random_xy(&end_x, &end_y);
	}
	while (color->start == color->end || color->start->color || color->end->color || get_distance(start_x, start_y, end_x, end_y) < distance_min);
	set_cell_color(color->start, color);
	set_cell_color(color->end, color);
}

unsigned long random_xy(unsigned long *x, unsigned long *y) {
	*x = erand(cols_n);
	*y = erand(rows_n);
	return *x*rows_n+*y;
}

unsigned long erand(unsigned long values) {
	return (unsigned long)(rand()/(RAND_MAX+1.0)*(double)values);
}

unsigned long get_distance(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2) {
	if (x1 < x2) {
		if (y1 < y2) {
			return x2-x1+y2-y1;
		}
		return x2-x1+y1-y2;
	}
	if (y1 < y2) {
		return x1-x2+y2-y1;
	}
	return x1-x2+y1-y2;
}

void set_cell_color(cell_t *cell, color_t *color) {
	cell->color = color;
	cell->links_todo--;
}

void flowfree(void) {
	nodes_n = 0UL;
	solutions_n = 0UL;
	stack_calls_n = 0UL;
	stack_selections_n = 0UL;
	stack_chains_n = 0UL;
	stack_unrolls_n = 0UL;
	stack_rolls_n = 0UL;
	stack_unchains_n = 0UL;
	stack_selection(stack_selections+stack_selections_n, cells_header->next);
	stack_call(stack_calls, CALL_SELECTION);
	do {
		stack_calls_n--;
		perform_call(stack_calls+stack_calls_n);
	}
	while (stack_calls_n > 0UL);
}

void stack_call(call_t *call, call_t type) {
	*call = type;
	stack_calls_n++;
}

void perform_call(call_t *call) {
	switch (*call) {
	case CALL_SELECTION:
		stack_selections_n--;
		perform_selection(stack_selections+stack_selections_n);
		break;
	case CALL_CHAIN:
		stack_chains_n--;
		perform_chain(stack_chains+stack_chains_n);
		break;
	case CALL_UNROLL:
		stack_unrolls_n--;
		perform_unroll(stack_unrolls+stack_unrolls_n);
		break;
	case CALL_ROLL:
		stack_rolls_n--;
		perform_roll(stack_rolls+stack_rolls_n);
		break;
	case CALL_UNCHAIN:
		stack_unchains_n--;
		perform_unchain(stack_unchains+stack_unchains_n);
		break;
	}
}

void stack_selection(selection_t *selection, cell_t *cell) {
	selection->cell = cell;
	stack_selections_n++;
}

void perform_selection(selection_t *selection) {
	nodes_n++;
	if (cells_header->next != cells_header) {
		if (solutions_n <= solutions_max) {
			cell_t *cell_min = selection->cell, *cell;
			constraint_t constraint_min;
			set_constraint(cell_min, CHOICES_MAX, &constraint_min);
			for (cell = cell_min->next; cell != selection->cell && constraint_min.choices_n > 1UL; cell = cell->next) {
				constraint_t constraint;
				if (cell == cells_header) {
					continue;
				}
				if (cell->links_todo < cell_min->links_todo || (cell > cell_min && cell->links_todo == cell_min->links_todo)) {
					set_constraint(cell, constraint_min.choices_n, &constraint);
					if (constraint.choices_n < constraint_min.choices_n) {
						cell_min = cell;
						constraint_min = constraint;
					}
				}
				else {
					if (constraint_min.choices_n < CHOICES_MAX) {
						set_constraint(cell, constraint_min.choices_n+1, &constraint);
						if (constraint.choices_n <= constraint_min.choices_n) {
							cell_min = cell;
							constraint_min = constraint;
						}
					}
					else {
						set_constraint(cell, CHOICES_MAX, &constraint);
						cell_min = cell;
						constraint_min = constraint;
					}
				}
			}
			if (constraint_min.choices_n > 0UL) {
				unsigned long i;
				stack_chain(stack_chains+stack_chains_n, cell_min, constraint_min.options_n, constraint_min.options);
				stack_call(stack_calls+stack_calls_n, CALL_CHAIN);
				for (i = constraint_min.choices_n; i > 0UL; i--) {
					unsigned long j;
					for (j = 0UL; j < cell_min->links_todo; j++) {
						stack_unroll(stack_unrolls+stack_unrolls_n, cell_min, &constraint_min.choices[i-1UL][j]);
						stack_call(stack_calls+stack_calls_n, CALL_UNROLL);
					}
					if (constraint_min.choices_n == 1UL) {
						if (cell_min->next != cells_header) {
							stack_selection(stack_selections+stack_selections_n, cell_min->next);
						}
						else {
							stack_selection(stack_selections+stack_selections_n, cells_header->next);
						}
					}
					else {
						if (cell_min != cells_header->next) {
							stack_selection(stack_selections+stack_selections_n, cells_header->next);
						}
						else {
							stack_selection(stack_selections+stack_selections_n, cell_min->next);
						}
					}
					stack_call(stack_calls+stack_calls_n, CALL_SELECTION);
					for (; j > 0UL; j--) {
						stack_roll(stack_rolls+stack_rolls_n, cell_min, &constraint_min.choices[i-1UL][j-1UL]);
						stack_call(stack_calls+stack_calls_n, CALL_ROLL);
					}
				}
				stack_unchain(stack_unchains+stack_unchains_n, cell_min, constraint_min.options_n, constraint_min.options);
				stack_call(stack_calls+stack_calls_n, CALL_UNCHAIN);
			}
		}
	}
	else {
		solutions_n++;
		if (solutions_n == 1UL) {
			unsigned long i;
			puts("");
			if (distance_min > 0UL) {
				if (grid_rotated) {
					printf("%lu %lu %lu\n", colors_n, rows_n, cols_n);
				}
				else {
					printf("%lu %lu %lu\n", colors_n, cols_n, rows_n);
				}
			}
			for (i = 0UL; i < colors_n; i++) {
				print_color(colors+i);
			}
		}
		else if (solutions_n > solutions_max) {
			puts("\nToo many solutions");
		}
		fflush(stdout);
	}
}

void set_constraint(cell_t *cell, unsigned long choices_max, constraint_t *constraint) {
	constraint->options_n = set_options[cell->pipes](cell, constraint->options);
	if (cell->links_todo > 0UL) {
		unsigned long i;
		rolls_n = 0UL;
		for (i = 0UL; i < constraint->options_n; i++) {
			add_roll_option(cell, constraint->options+i);
		}
		constraint->choices_n = 0UL;
		if (rolls_n >= cell->links_todo) {
			add_choices(cell, choices_max, 0UL, 0UL, 0UL, &constraint->choices_n, constraint->choices);
		}
	}
	else {
		constraint->choices_n = 1UL;
	}
}

unsigned long set_options_empty(cell_t *cell, option_t options[]) {
	set_option(options, 0UL, cell);
	return 0UL;
}

unsigned long set_options_w(cell_t *cell, option_t options[]) {
	set_option(options, PIPE_W, cell-rows_n);
	return 1UL;
}

unsigned long set_options_n(cell_t *cell, option_t options[]) {
	set_option(options, PIPE_N, cell-1UL);
	return 1UL;
}

unsigned long set_options_wn(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options, PIPE_N, cell-1UL);
	return 2UL;
}

unsigned long set_options_s(cell_t *cell, option_t options[]) {
	set_option(options, PIPE_S, cell+1UL);
	return 1UL;
}

unsigned long set_options_ws(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options, PIPE_S, cell+1UL);
	return 2UL;
}

unsigned long set_options_ns(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_N, cell-1UL);
	set_option(options, PIPE_S, cell+1UL);
	return 2UL;
}

unsigned long set_options_wns(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options++, PIPE_N, cell-1UL);
	set_option(options, PIPE_S, cell+1UL);
	return 3UL;
}

unsigned long set_options_e(cell_t *cell, option_t options[]) {
	set_option(options, PIPE_E, cell+rows_n);
	return 1UL;
}

unsigned long set_options_we(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options, PIPE_E, cell+rows_n);
	return 2UL;
}

unsigned long set_options_ne(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_N, cell-1UL);
	set_option(options, PIPE_E, cell+rows_n);
	return 2UL;
}

unsigned long set_options_wne(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options++, PIPE_N, cell-1UL);
	set_option(options, PIPE_E, cell+rows_n);
	return 3UL;
}

unsigned long set_options_se(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_S, cell+1UL);
	set_option(options, PIPE_E, cell+rows_n);
	return 2UL;
}

unsigned long set_options_wse(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options++, PIPE_S, cell+1UL);
	set_option(options, PIPE_E, cell+rows_n);
	return 3UL;
}

unsigned long set_options_nse(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_N, cell-1UL);
	set_option(options++, PIPE_S, cell+1UL);
	set_option(options, PIPE_E, cell+rows_n);
	return 3UL;
}

unsigned long set_options_wnse(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options++, PIPE_N, cell-1UL);
	set_option(options++, PIPE_S, cell+1UL);
	set_option(options, PIPE_E, cell+rows_n);
	return 4UL;
}

void set_option(option_t *option, unsigned long pipe_used, cell_t *link) {
	option->pipe_used = pipe_used;
	option->link = link;
}

void add_roll_option(cell_t *cell, option_t *option) {
	if (option->link->links_todo > 0UL && roll_option(cell, option->link)) {
		rolls[rolls_n++] = option;
	}
}

void add_choices(cell_t *cell, unsigned long choices_max, unsigned long step, unsigned long rolls_start, unsigned long pipes, unsigned long *choices_n, choice_t choices[][LINKS_MAX]) {
	if (step < cell->links_todo) {
		unsigned long i;
		for (i = rolls_start; i < rolls_n && *choices_n < choices_max; i++) {
			unsigned long j;
			for (j = 0UL; j < step && roll_option(rolls[rolls_idx[j]]->link, rolls[i]->link); j++);
			if (j == step) {
				rolls_idx[step] = i;
				add_choices(cell, choices_max, step+1UL, i+1UL, pipes+rolls[i]->pipe_used, choices_n, choices);
			}
		}
	}
	else {
		if (touching_allowed || !touching[pipes](cell)) {
			unsigned long i;
			for (i = 0UL; i < cell->links_todo; i++) {
				choices[*choices_n][i].link = rolls[rolls_idx[i]]->link;
			}
			*choices_n = *choices_n+1UL;
		}
	}
}

int roll_option(cell_t *cell, cell_t *link) {
	if (link->path == cell->path) {
		return 0;
	}
	if (link->path->color && cell->path->color) {
		return link->path->color == cell->path->color;
	}
	return 1;
}

int no_touching(cell_t *cell) {
	return cell == NULL;
}

int touching_w(cell_t *cell) {
	if (cell->row > 0UL) {
		if (same_path(cell, cell-rows_n-1UL) || same_path(cell-rows_n, cell-1UL)) {
			return 1;
		}
		if (same_path(cell, cell-1UL) && same_path(cell-rows_n, cell-rows_n-1UL)) {
			return 1;
		}
	}
	if (cell->row < rows_n-1UL) {
		if (same_path(cell, cell-rows_n+1UL) || same_path(cell-rows_n, cell+1UL)) {
			return 1;
		}
		if (same_path(cell, cell+1UL) && same_path(cell-rows_n, cell-rows_n+1UL)) {
			return 1;
		}
	}
	return 0;
}

int touching_n(cell_t *cell) {
	if (cell->col > 0UL) {
		if (same_path(cell, cell-rows_n-1UL) || same_path(cell-1UL, cell-rows_n)) {
			return 1;
		}
		if (same_path(cell, cell-rows_n) && same_path(cell-1UL, cell-rows_n-1UL)) {
			return 1;
		}
	}
	if (cell->col < cols_n-1UL) {
		if (same_path(cell, cell+rows_n-1UL) || same_path(cell-1UL, cell+rows_n)) {
			return 1;
		}
		if (same_path(cell, cell+rows_n) && same_path(cell-1UL, cell+rows_n-1UL)) {
			return 1;
		}
	}
	return 0;
}

int touching_wn(cell_t *cell) {
	if (same_path(cell-rows_n, cell-rows_n-1UL) || same_path(cell-1UL, cell-rows_n-1UL)) {
		return 1;
	}
	if (cell->row < rows_n-1UL && same_path(cell-rows_n, cell+1UL)) {
		return 1;
	}
	if (cell->col < cols_n-1UL && same_path(cell-1UL, cell+rows_n)) {
		return 1;
	}
	return 0;
}

int touching_s(cell_t *cell) {
	if (cell->col > 0UL) {
		if (same_path(cell, cell-rows_n+1UL) || same_path(cell+1UL, cell-rows_n)) {
			return 1;
		}
		if (same_path(cell, cell-rows_n) && same_path(cell+1UL, cell-rows_n+1UL)) {
			return 1;
		}
	}
	if (cell->col < cols_n-1UL) {
		if (same_path(cell, cell+rows_n+1UL) || same_path(cell+1UL, cell+rows_n)) {
			return 1;
		}
		if (same_path(cell, cell+rows_n) && same_path(cell+1UL, cell+rows_n+1UL)) {
			return 1;
		}
	}
	return 0;
}

int touching_ws(cell_t *cell) {
	if (same_path(cell-rows_n, cell-rows_n+1UL) || same_path(cell+1UL, cell-rows_n+1UL)) {
		return 1;
	}
	if (cell->row > 0UL && same_path(cell-rows_n, cell-1UL)) {
		return 1;
	}
	if (cell->col < cols_n-1UL && same_path(cell+1UL, cell+rows_n)) {
		return 1;
	}
	return 0;
}

int touching_ns(cell_t *cell) {
	if (cell->col > 0UL) {
		if (same_path(cell-1UL, cell-rows_n) || same_path(cell-1UL, cell-rows_n+1UL) || same_path(cell+1UL, cell-rows_n-1UL) || same_path(cell+1UL, cell-rows_n)) {
			return 1;
		}
	}
	if (cell->col < cols_n-1UL) {
		if (same_path(cell-1UL, cell+rows_n) || same_path(cell-1UL, cell+rows_n+1UL) || same_path(cell+1UL, cell+rows_n-1UL) || same_path(cell+1UL, cell+rows_n)) {
			return 1;
		}
	}
	return 0;
}

int touching_e(cell_t *cell) {
	if (cell->row > 0UL) {
		if (same_path(cell, cell+rows_n-1UL) || same_path(cell+rows_n, cell-1UL)) {
			return 1;
		}
		if (same_path(cell, cell-1UL) && same_path(cell+rows_n, cell+rows_n-1UL)) {
			return 1;
		}
	}
	if (cell->row < rows_n-1UL) {
		if (same_path(cell, cell+rows_n+1UL) || same_path(cell+rows_n, cell+1UL)) {
			return 1;
		}
		if (same_path(cell, cell+1UL) && same_path(cell+rows_n, cell+rows_n+1UL)) {
			return 1;
		}
	}
	return 0;
}

int touching_we(cell_t *cell) {
	if (cell->row > 0UL) {
		if (same_path(cell-rows_n, cell-1UL) || same_path(cell-rows_n, cell+rows_n-1UL) || same_path(cell+rows_n, cell-rows_n-1UL) || same_path(cell+rows_n, cell-1UL)) {
			return 1;
		}
	}
	if (cell->row < rows_n-1UL) {
		if (same_path(cell-rows_n, cell+1UL) || same_path(cell-rows_n, cell+rows_n+1UL) || same_path(cell+rows_n, cell-rows_n+1UL) || same_path(cell+rows_n, cell+1UL)) {
			return 1;
		}
	}
	return 0;
}

int touching_ne(cell_t *cell) {
	if (same_path(cell-1UL, cell+rows_n-1UL) || same_path(cell+rows_n, cell+rows_n-1UL)) {
		return 1;
	}
	if (cell->col > 0UL && same_path(cell-1UL, cell-rows_n)) {
		return 1;
	}
	if (cell->row < rows_n-1UL && same_path(cell+rows_n, cell+1UL)) {
		return 1;
	}
	return 0;
}

int touching_se(cell_t *cell) {
	if (same_path(cell+rows_n, cell+rows_n+1UL) || same_path(cell+1UL, cell+rows_n+1UL)) {
		return 1;
	}
	if (cell->col > 0UL && same_path(cell+1UL, cell-rows_n)) {
		return 1;
	}
	if (cell->row > 0UL && same_path(cell+rows_n, cell-1UL)) {
		return 1;
	}
	return 0;
}

int same_path(cell_t *cell_a, cell_t *cell_b) {
	return cell_a->path == cell_b->path;
}

void stack_chain(chain_t *chain, cell_t *cell, unsigned long options_n, option_t options[]) {
	unsigned long i;
	chain->cell = cell;
	chain->options_n = options_n;
	for (i = 0UL; i < options_n; i++) {
		chain->options[i] = options[i];
	}
	stack_chains_n++;
}

void stack_unroll(unroll_t *unroll, cell_t *cell, choice_t *choice) {
	choice->unroll = unroll;
	unroll->cell = cell;
	unroll->link = choice->link;
	stack_unrolls_n++;
}

void stack_roll(roll_t *roll, cell_t *cell, choice_t *choice) {
	roll->cell = cell;
	roll->link = choice->link;
	roll->unroll = choice->unroll;
	stack_rolls_n++;
}

void stack_unchain(chain_t *unchain, cell_t *cell, unsigned long options_n, option_t options[]) {
	unsigned long i;
	unchain->cell = cell;
	unchain->options_n = options_n;
	for (i = 0UL; i < options_n; i++) {
		unchain->options[i] = options[i];
	}
	stack_unchains_n++;
}

void print_color(color_t *color) {
	if (distance_min > 0UL) {
		print_cell(color->start, ' ');
		print_cell(color->end, '\n');
	}
	else {
		cell_t *cell = color->start, *next = next_cell(cell, NULL);
		print_cell(cell, ' ');
		while (next != color->end) {
			cell_t *last = cell;
			cell = next;
			next = next_cell(cell, last);
			print_cell(cell, ' ');
		}
		print_cell(next, '\n');
	}
}

void print_cell(cell_t *cell, int c) {
	if (grid_rotated) {
		printf("(%lu, %lu)", cell->row, cell->col);
	}
	else {
		printf("(%lu, %lu)", cell->col, cell->row);
	}
	putchar(c);
}

void perform_chain(chain_t *chain) {
	unsigned long i;
	for (i = 0UL; i < chain->options_n; i++) {
		add_pipe(chain->cell, chain->options[i].pipe_used, chain->options[i].link);
	}
	chain_cell(chain->cell);
}

void add_pipe(cell_t *cell, unsigned long pipe_used, cell_t *link) {
	link->pipes += PIPE_E/pipe_used;
	cell->pipes += pipe_used;
}

void chain_cell(cell_t *cell) {
	cell->last->next = cell;
	cell->next->last = cell;
}

void perform_unroll(unroll_t *unroll) {
	unroll_link(unroll->cell, unroll->link, unroll->cell_path, unroll->cell_color, unroll->link_path, unroll->link_color);
}

void unroll_link(cell_t *cell, cell_t *link, cell_t *cell_path, color_t *cell_color, cell_t *link_path, color_t *link_color) {
	link->links[--link->links_done] = NULL;
	link->links_todo++;
	cell->links[--cell->links_done] = NULL;
	cell->links_todo++;
	if (cell_path->rank < link_path->rank) {
		update_path(cell, cell_path);
		link_path->rank -= cell_path->rank;
	}
	else {
		update_path(link, link_path);
		cell_path->rank -= link_path->rank;
	}
	if (cell_color) {
		if (!link_color) {
			link_path->color = NULL;
		}
	}
	else {
		if (link_color) {
			cell_path->color = NULL;
		}
	}
}

void perform_roll(roll_t *roll) {
	roll_link(roll->cell, roll->link, roll->unroll);
}

void roll_link(cell_t *cell, cell_t *link, unroll_t *unroll) {
	unroll->cell_path = cell->path;
	unroll->cell_color = cell->path->color;
	unroll->link_path = link->path;
	unroll->link_color = link->path->color;
	if (cell->path->color) {
		if (!link->path->color) {
			link->path->color = cell->path->color;
		}
	}
	else {
		if (link->path->color) {
			cell->path->color = link->path->color;
		}
	}
	if (cell->path->rank < link->path->rank) {
		link->path->rank += cell->path->rank;
		update_path(cell, link->path);
	}
	else {
		cell->path->rank += link->path->rank;
		update_path(link, cell->path);
	}
	cell->links_todo--;
	cell->links[cell->links_done++] = link;
	link->links_todo--;
	link->links[link->links_done++] = cell;
}

void perform_unchain(chain_t *unchain) {
	unsigned long i;
	unchain_cell(unchain->cell);
	for (i = 0UL; i < unchain->options_n; i++) {
		remove_pipe(unchain->cell, unchain->options[i].pipe_used, unchain->options[i].link);
	}
}

void unchain_cell(cell_t *cell) {
	cell->last->next = cell->next;
	cell->next->last = cell->last;
}

void remove_pipe(cell_t *cell, unsigned long pipe_used, cell_t *link) {
	cell->pipes -= pipe_used;
	link->pipes -= PIPE_E/pipe_used;
}

void update_path(cell_t *start, cell_t *path) {
	cell_t *cell = start, *next = next_cell(cell, NULL);
	cell->path = path;
	while (next) {
		cell_t *last = cell;
		cell = next;
		next = next_cell(cell, last);
		cell->path = path;
	}
}

cell_t *next_cell(cell_t *cell, cell_t *last) {
	unsigned long i;
	for (i = 0UL; i < cell->links_done && cell->links[i] == last; i++);
	if (i < cell->links_done) {
		return cell->links[i];
	}
	return NULL;
}

void reset_color(color_t *color) {
	color->start->color = NULL;
	color->start->links_todo++;
	color->end->color = NULL;
	color->end->links_todo++;
}
