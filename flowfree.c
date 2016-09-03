#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define LINKS_MAX 2
#define OPTIONS_MAX 4
#define CHOICES_MAX 2
#define PIPE_W 1UL
#define PIPE_N 2UL
#define PIPE_S 4UL
#define PIPE_E 8UL
#define CALLS_MAX 11
#define ROLLS_MAX 4

typedef struct cell_s cell_t;
typedef struct color_s color_t;

struct cell_s {
	unsigned long column;
	unsigned long row;
	unsigned long pipes;
	cell_t *path;
	unsigned long rank;
	color_t *color;
	unsigned long links_max;
	unsigned long links_n;
	cell_t *links[LINKS_MAX];
	cell_t *last;
	cell_t *next;
};

struct color_s {
	cell_t *start;
	cell_t *end;
};

typedef enum {
	CALL_CHOOSE,
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
void perform_choose(void);
void print_color(color_t *);
void print_cell(cell_t *, int);
void set_constraint(cell_t *, constraint_t *);
unsigned long set_options_empty(cell_t *, option_t []);
unsigned long set_options_w(cell_t *, option_t []);
unsigned long set_options_n(cell_t *, option_t []);
unsigned long set_options_s(cell_t *, option_t []);
unsigned long set_options_wn(cell_t *, option_t []);
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
void add_roll_option(cell_t *, option_t *, unsigned long *, option_t *[]);
void add_choices(cell_t *, option_t *[], unsigned long [], unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long *, choice_t [][LINKS_MAX]);
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

int touching_allowed, (*touching[])(cell_t *) = { no_touching, touching_w, touching_n, touching_wn, touching_s, touching_ws, touching_ns, no_touching, touching_e, touching_we, touching_ne, no_touching, touching_se, no_touching, no_touching, no_touching };
unsigned long colors_n, columns_n, rows_n, distance_min, solutions_max, nodes_n, solutions_n, stack_calls_n, stack_chains_n, stack_unrolls_n, stack_rolls_n, stack_unchains_n, (*set_options[])(cell_t *, option_t []) = { set_options_empty, set_options_w, set_options_n, set_options_wn, set_options_s, set_options_ws, set_options_ns, set_options_wns, set_options_e, set_options_we, set_options_ne, set_options_wne, set_options_se, set_options_wse, set_options_nse, set_options_wnse };
cell_t *cells, *cell_header;
color_t *colors;
call_t *stack_calls;
chain_t *stack_chains, *stack_unchains;
unroll_t *stack_unrolls;
roll_t *stack_rolls;

int main(void) {
unsigned long attempts_n_mod, cells_n, attempts_n, i, j;
cell_t *cell;
	if (scanf("%lu%lu%lu", &colors_n, &columns_n, &rows_n) != 3 || !colors_n || columns_n < 2 || rows_n < 2 || columns_n*rows_n < colors_n*2) {
		fprintf(stderr, "Invalid parameters\n");
		return EXIT_FAILURE;
	}
	cells_n = columns_n*rows_n;
	cells = malloc(sizeof(cell_t)*(cells_n+1));
	if (!cells) {
		fprintf(stderr, "Could not allocate memory for cells\n");
		return EXIT_FAILURE;
	}
	set_cell(cells, 0UL, 0UL, PIPE_E+PIPE_S);
	cell = cells+1;
	for (j = 1; j < rows_n-1; j++) {
		set_cell(cell++, 0UL, j, PIPE_N+PIPE_E+PIPE_S);
	}
	set_cell(cell++, 0UL, j, PIPE_N+PIPE_E);
	for (i = 1; i < columns_n-1; i++) {
		set_cell(cell++, i, 0UL, PIPE_W+PIPE_E+PIPE_S);
		for (j = 1; j < rows_n-1; j++) {
			set_cell(cell++, i, j, PIPE_W+PIPE_N+PIPE_E+PIPE_S);
		}
		set_cell(cell++, i, j, PIPE_W+PIPE_N+PIPE_E);
	}
	set_cell(cell++, i, 0UL, PIPE_W+PIPE_S);
	for (j = 1; j < rows_n-1; j++) {
		set_cell(cell++, i, j, PIPE_W+PIPE_N+PIPE_S);
	}
	set_cell(cell, i, j, PIPE_W+PIPE_N);
	cell_header = cell+1;
	link_cell(cells, cell_header, cells+1);
	for (cell = cells+1; cell < cell_header; cell++) {
		link_cell(cell, cell-1, cell+1);
	}
	link_cell(cell, cell-1, cells);
	colors = malloc(sizeof(color_t)*colors_n);
	if (!colors) {
		fprintf(stderr, "Could not allocate memory for colors\n");
		free(cells);
		return EXIT_FAILURE;
	}
	if (scanf("%d", &touching_allowed) == 1) {
		if (scanf("%lu", &distance_min) != 1 || !distance_min || distance_min > columns_n+rows_n-colors_n-1 || scanf("%lu%lu", &solutions_max, &attempts_n_mod) != 2 || !solutions_max || !attempts_n_mod) {
			fprintf(stderr, "Invalid generator parameters\n");
			return EXIT_FAILURE;
		}
	}
	else {
		distance_min = 0;
		solutions_max = ULONG_MAX;
		attempts_n_mod = 1;
		for (i = 0; i < colors_n; i++) {
			if (!read_color(colors+i)) {
				free(colors);
				free(cells);
				return EXIT_FAILURE;
			}
		}
	}
	stack_calls = malloc(sizeof(call_t)*(cells_n*CALLS_MAX+1));
	if (!stack_calls) {
		fprintf(stderr, "Could not allocate memory for calls stack\n");
		free(colors);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_chains = malloc(sizeof(chain_t)*cells_n);
	if (!stack_chains) {
		fprintf(stderr, "Could not allocate memory for chains stack\n");
		free(stack_calls);
		free(colors);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_unrolls = malloc(sizeof(unroll_t)*cells_n*ROLLS_MAX);
	if (!stack_unrolls) {
		fprintf(stderr, "Could not allocate memory for unrolls stack\n");
		free(stack_chains);
		free(stack_calls);
		free(colors);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_rolls = malloc(sizeof(roll_t)*cells_n*ROLLS_MAX);
	if (!stack_rolls) {
		fprintf(stderr, "Could not allocate memory for rolls stack\n");
		free(stack_unrolls);
		free(stack_chains);
		free(stack_calls);
		free(colors);
		free(cells);
		return EXIT_FAILURE;
	}
	stack_unchains = malloc(sizeof(chain_t)*cells_n);
	if (!stack_unchains) {
		fprintf(stderr, "Could not allocate memory for unchains stack\n");
		free(stack_rolls);
		free(stack_unrolls);
		free(stack_chains);
		free(stack_calls);
		free(colors);
		free(cells);
		return EXIT_FAILURE;
	}
	if (distance_min) {
		srand((unsigned)time(NULL));
		attempts_n = 0;
		do {
			for (i = 0; i < colors_n; i++) {
				set_color(colors+i);
			}
			flowfree();
			for (i = 0; i < colors_n; i++) {
				reset_color(colors+i);
			}
			attempts_n++;
			if (attempts_n%attempts_n_mod == 0) {
				printf("Attempts %lu\n", attempts_n);
			}
		}
		while (!solutions_n || solutions_n > solutions_max);
		printf("\nNodes %lu\nSolutions %lu\nAttempts %lu\n", nodes_n, solutions_n, attempts_n);
	}
	else {
		touching_allowed = 0;
		flowfree();
		printf("\nTouching forbidden\nNodes %lu\nSolutions %lu\n", nodes_n, solutions_n);
		if (!solutions_n) {
			touching_allowed = 1;
			flowfree();
			printf("\nTouching allowed\nNodes %lu\nSolutions %lu\n", nodes_n, solutions_n);
		}
	}
	free(stack_unchains);
	free(stack_rolls);
	free(stack_unrolls);
	free(stack_chains);
	free(stack_calls);
	free(colors);
	free(cells);
	return EXIT_SUCCESS;
}

void set_cell(cell_t *cell, unsigned long column, unsigned long row, unsigned long pipes) {
unsigned long i;
	cell->column = column;
	cell->row = row;
	cell->pipes = pipes;
	cell->path = cell;
	cell->rank = 1;
	cell->color = NULL;
	cell->links_max = LINKS_MAX;
	cell->links_n = 0;
	for (i = 0; i < LINKS_MAX; i++) {
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
		return 0;
	}
	if (color->start->color) {
		fprintf(stderr, "Start cell color already set\n");
		return 0;
	}
	color->end = read_cell('\n');
	if (!color->end) {
		fprintf(stderr, "Invalid end cell\n");
		return 0;
	}
	if (color->end->color) {
		fprintf(stderr, "End cell color already set\n");
		return 0;
	}
	set_cell_color(color->start, color);
	set_cell_color(color->end, color);
	return 1;
}

cell_t *read_cell(int c) {
unsigned long x, y;
	if (scanf("(%lu, %lu)", &x, &y) != 2 || x > columns_n || y > rows_n || fgetc(stdin) != c) {
		return NULL;
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
	*x = erand(columns_n);
	*y = erand(rows_n);
	return *x*rows_n+*y;
}

unsigned long erand(unsigned long values) {
	return (unsigned long)(rand()/(RAND_MAX+1.0)*values);
}

unsigned long get_distance(unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2) {
	if (x1 < x2) {
		if (y1 < y2) {
			return x2-x1+y2-y1;
		}
		else {
			return x2-x1+y1-y2;
		}
	}
	else {
		if (y1 < y2) {
			return x1-x2+y2-y1;
		}
		else {
			return x1-x2+y1-y2;
		}
	}
}

void set_cell_color(cell_t *cell, color_t *color) {
	cell->color = color;
	cell->links_max--;
}

void flowfree(void) {
	nodes_n = 0;
	solutions_n = 0;
	stack_calls_n = 0;
	stack_chains_n = 0;
	stack_unrolls_n = 0;
	stack_rolls_n = 0;
	stack_unchains_n = 0;
	stack_call(stack_calls, CALL_CHOOSE);
	do {
		stack_calls_n--;
		perform_call(stack_calls+stack_calls_n);
	}
	while (stack_calls_n);
}

void stack_call(call_t *call, call_t type) {
	*call = type;
	stack_calls_n++;
}

void perform_call(call_t *call) {
	switch (*call) {
	case CALL_CHOOSE:
		perform_choose();
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

void perform_choose(void) {
unsigned long i, j;
cell_t *cell_min, *cell;
constraint_t constraint_min, constraint;
	nodes_n++;
	if (cell_header->next == cell_header) {
		solutions_n++;
		if (solutions_n == 1) {
			puts("");
			if (distance_min) {
				printf("%lu %lu %lu\n", colors_n, columns_n, rows_n);
			}
			for (i = 0; i < colors_n; i++) {
				print_color(colors+i);
			}
		}
		else if (solutions_n > solutions_max) {
			puts("\nToo many solutions");
		}
	}
	else {
		cell_min = cell_header->next;
		set_constraint(cell_min, &constraint_min);
		for (cell = cell_min->next; cell != cell_header && constraint_min.choices_n > 1; cell = cell->next) {
			set_constraint(cell, &constraint);
			if (constraint.choices_n < constraint_min.choices_n) {
				cell_min = cell;
				constraint_min = constraint;
			}
		}
		if (constraint_min.choices_n && solutions_n <= solutions_max) {
			stack_chain(stack_chains+stack_chains_n, cell_min, constraint_min.options_n, constraint_min.options);
			stack_call(stack_calls+stack_calls_n, CALL_CHAIN);
			for (i = constraint_min.choices_n; i; i--) {
				for (j = 0; j < cell_min->links_max; j++) {
					stack_unroll(stack_unrolls+stack_unrolls_n, cell_min, &constraint_min.choices[i-1][j]);
					stack_call(stack_calls+stack_calls_n, CALL_UNROLL);
				}
				stack_call(stack_calls+stack_calls_n, CALL_CHOOSE);
				for (j = cell_min->links_max; j; j--) {
					stack_roll(stack_rolls+stack_rolls_n, cell_min, &constraint_min.choices[i-1][j-1]);
					stack_call(stack_calls+stack_calls_n, CALL_ROLL);
				}
			}
			stack_unchain(stack_unchains+stack_unchains_n, cell_min, constraint_min.options_n, constraint_min.options);
			stack_call(stack_calls+stack_calls_n, CALL_UNCHAIN);
		}
	}
}

void print_color(color_t *color) {
cell_t *cell, *next, *last;
	if (distance_min) {
		print_cell(color->start, ' ');
		print_cell(color->end, '\n');
	}
	else {
		cell = color->start;
		next = next_cell(cell, NULL);
		print_cell(cell, ' ');
		while (next != color->end) {
			last = cell;
			cell = next;
			next = next_cell(cell, last);
			print_cell(cell, ' ');
		}
		print_cell(next, '\n');
	}
}

void print_cell(cell_t *cell, int c) {
	printf("(%lu, %lu)", cell->column, cell->row);
	putchar(c);
}

void set_constraint(cell_t *cell, constraint_t *constraint) {
unsigned long rolls_n, rolls_idx[LINKS_MAX], i;
option_t *rolls[OPTIONS_MAX];
	constraint->options_n = set_options[cell->pipes](cell, constraint->options);
	if (!cell->links_max) {
		constraint->choices_n = 1;
	}
	else {
		rolls_n = 0;
		for (i = 0; i < constraint->options_n; i++) {
			add_roll_option(cell, constraint->options+i, &rolls_n, rolls);
		}
		constraint->choices_n = 0;
		if (rolls_n >= cell->links_max) {
			add_choices(cell, rolls, rolls_idx, 0UL, cell->links_max, 0UL, rolls_n, 0UL, &constraint->choices_n, constraint->choices);
		}
	}
}

unsigned long set_options_empty(cell_t *cell, option_t options[]) {
	set_option(options, 0UL, cell);
	return 0;
}

unsigned long set_options_w(cell_t *cell, option_t options[]) {
	set_option(options, PIPE_W, cell-rows_n);
	return 1;
}

unsigned long set_options_n(cell_t *cell, option_t options[]) {
	set_option(options, PIPE_N, cell-1);
	return 1;
}

unsigned long set_options_wn(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options, PIPE_N, cell-1);
	return 2;
}

unsigned long set_options_s(cell_t *cell, option_t options[]) {
	set_option(options, PIPE_S, cell+1);
	return 1;
}

unsigned long set_options_ws(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options, PIPE_S, cell+1);
	return 2;
}

unsigned long set_options_ns(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_N, cell-1);
	set_option(options, PIPE_S, cell+1);
	return 2;
}

unsigned long set_options_wns(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options++, PIPE_N, cell-1);
	set_option(options, PIPE_S, cell+1);
	return 3;
}

unsigned long set_options_e(cell_t *cell, option_t options[]) {
	set_option(options, PIPE_E, cell+rows_n);
	return 1;
}

unsigned long set_options_we(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options, PIPE_E, cell+rows_n);
	return 2;
}

unsigned long set_options_ne(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_N, cell-1);
	set_option(options, PIPE_E, cell+rows_n);
	return 2;
}

unsigned long set_options_wne(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options++, PIPE_N, cell-1);
	set_option(options, PIPE_E, cell+rows_n);
	return 3;
}

unsigned long set_options_se(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_S, cell+1);
	set_option(options, PIPE_E, cell+rows_n);
	return 2;
}

unsigned long set_options_wse(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options++, PIPE_S, cell+1);
	set_option(options, PIPE_E, cell+rows_n);
	return 3;
}

unsigned long set_options_nse(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_N, cell-1);
	set_option(options++, PIPE_S, cell+1);
	set_option(options, PIPE_E, cell+rows_n);
	return 3;
}

unsigned long set_options_wnse(cell_t *cell, option_t options[]) {
	set_option(options++, PIPE_W, cell-rows_n);
	set_option(options++, PIPE_N, cell-1);
	set_option(options++, PIPE_S, cell+1);
	set_option(options, PIPE_E, cell+rows_n);
	return 4;
}

void set_option(option_t *option, unsigned long pipe_used, cell_t *link) {
	option->pipe_used = pipe_used;
	option->link = link;
}

void add_roll_option(cell_t *cell, option_t *option, unsigned long *rolls_n, option_t *rolls[]) {
	if (option->link->links_max && roll_option(cell, option->link)) {
		rolls[*rolls_n] = option;
		*rolls_n = *rolls_n+1;
	}
}

void add_choices(cell_t *cell, option_t *rolls[], unsigned long rolls_idx[], unsigned long step, unsigned long step_max, unsigned long rolls_start, unsigned long rolls_n, unsigned long pipes, unsigned long *choices_n, choice_t choices[][LINKS_MAX]) {
unsigned long i, j;
	if (step < step_max) {
		for (i = rolls_start; i < rolls_n && *choices_n < CHOICES_MAX; i++) {
			for (j = 0; j < step && roll_option(rolls[rolls_idx[j]]->link, rolls[i]->link); j++);
			if (j == step) {
				rolls_idx[step] = i;
				add_choices(cell, rolls, rolls_idx, step+1, step_max, i+1, rolls_n, pipes+rolls[i]->pipe_used, choices_n, choices);
			}
		}
	}
	else {
		if (touching_allowed || !touching[pipes](cell)) {
			for (i = 0; i < step_max; i++) {
				choices[*choices_n][i].link = rolls[rolls_idx[i]]->link;
			}
			*choices_n = *choices_n+1;
		}
	}
}

int roll_option(cell_t *cell, cell_t *link) {
	if (link->path == cell->path) {
		return 0;
	}
	else {
		if (link->path->color) {
			if (cell->path->color) {
				return link->path->color == cell->path->color;
			}
			else {
				return 1;
			}
		}
		else {
			return 1;
		}
	}
}

int no_touching(cell_t *cell) {
	return cell == NULL;
}

int touching_w(cell_t *cell) {
	if (cell->row) {
		if (same_path(cell, cell-rows_n-1) || same_path(cell-rows_n, cell-1)) {
			return 1;
		}
		if (same_path(cell, cell-1) && same_path(cell-rows_n, cell-rows_n-1)) {
			return 1;
		}
	}
	if (cell->row < rows_n-1) {
		if (same_path(cell, cell-rows_n+1) || same_path(cell-rows_n, cell+1)) {
			return 1;
		}
		if (same_path(cell, cell+1) && same_path(cell-rows_n, cell-rows_n+1)) {
			return 1;
		}
	}
	return 0;
}

int touching_n(cell_t *cell) {
	if (cell->column) {
		if (same_path(cell, cell-rows_n-1) || same_path(cell-1, cell-rows_n)) {
			return 1;
		}
		if (same_path(cell, cell-rows_n) && same_path(cell-1, cell-rows_n-1)) {
			return 1;
		}
	}
	if (cell->column < columns_n-1) {
		if (same_path(cell, cell+rows_n-1) || same_path(cell-1, cell+rows_n)) {
			return 1;
		}
		if (same_path(cell, cell+rows_n) && same_path(cell-1, cell+rows_n-1)) {
			return 1;
		}
	}
	return 0;
}

int touching_wn(cell_t *cell) {
	if (same_path(cell-rows_n, cell-rows_n-1) || same_path(cell-1, cell-rows_n-1)) {
		return 1;
	}
	if (cell->row < rows_n-1 && same_path(cell-rows_n, cell+1)) {
		return 1;
	}
	if (cell->column < columns_n-1 && same_path(cell-1, cell+rows_n)) {
		return 1;
	}
	return 0;
}

int touching_s(cell_t *cell) {
	if (cell->column) {
		if (same_path(cell, cell-rows_n+1) || same_path(cell+1, cell-rows_n)) {
			return 1;
		}
		if (same_path(cell, cell-rows_n) && same_path(cell+1, cell-rows_n+1)) {
			return 1;
		}
	}
	if (cell->column < columns_n-1) {
		if (same_path(cell, cell+rows_n+1) || same_path(cell+1, cell+rows_n)) {
			return 1;
		}
		if (same_path(cell, cell+rows_n) && same_path(cell+1, cell+rows_n+1)) {
			return 1;
		}
	}
	return 0;
}

int touching_ws(cell_t *cell) {
	if (same_path(cell-rows_n, cell-rows_n+1) || same_path(cell+1, cell-rows_n+1)) {
		return 1;
	}
	if (cell->row && same_path(cell-rows_n, cell-1)) {
		return 1;
	}
	if (cell->column < columns_n-1 && same_path(cell+1, cell+rows_n)) {
		return 1;
	}
	return 0;
}

int touching_ns(cell_t *cell) {
	if (cell->column) {
		if (same_path(cell-1, cell-rows_n) || same_path(cell-1, cell-rows_n+1) || same_path(cell+1, cell-rows_n-1) || same_path(cell+1, cell-rows_n)) {
			return 1;
		}
	}
	if (cell->column < columns_n-1) {
		if (same_path(cell-1, cell+rows_n) || same_path(cell-1, cell+rows_n+1) || same_path(cell+1, cell+rows_n-1) || same_path(cell+1, cell+rows_n)) {
			return 1;
		}
	}
	return 0;
}

int touching_e(cell_t *cell) {
	if (cell->row) {
		if (same_path(cell, cell+rows_n-1) || same_path(cell+rows_n, cell-1)) {
			return 1;
		}
		if (same_path(cell, cell-1) && same_path(cell+rows_n, cell+rows_n-1)) {
			return 1;
		}
	}
	if (cell->row < rows_n-1) {
		if (same_path(cell, cell+rows_n+1) || same_path(cell+rows_n, cell+1)) {
			return 1;
		}
		if (same_path(cell, cell+1) && same_path(cell+rows_n, cell+rows_n+1)) {
			return 1;
		}
	}
	return 0;
}

int touching_we(cell_t *cell) {
	if (cell->row) {
		if (same_path(cell-rows_n, cell-1) || same_path(cell-rows_n, cell+rows_n-1) || same_path(cell+rows_n, cell-rows_n-1) || same_path(cell+rows_n, cell-1)) {
			return 1;
		}
	}
	if (cell->row < rows_n-1) {
		if (same_path(cell-rows_n, cell+1) || same_path(cell-rows_n, cell+rows_n+1) || same_path(cell+rows_n, cell-rows_n+1) || same_path(cell+rows_n, cell+1)) {
			return 1;
		}
	}
	return 0;
}

int touching_ne(cell_t *cell) {
	if (same_path(cell-1, cell+rows_n-1) || same_path(cell+rows_n, cell+rows_n-1)) {
		return 1;
	}
	if (cell->column && same_path(cell-1, cell-rows_n)) {
		return 1;
	}
	if (cell->row < rows_n-1 && same_path(cell+rows_n, cell+1)) {
		return 1;
	}
	return 0;
}

int touching_se(cell_t *cell) {
	if (same_path(cell+rows_n, cell+rows_n+1) || same_path(cell+1, cell+rows_n+1)) {
		return 1;
	}
	if (cell->column && same_path(cell+1, cell-rows_n)) {
		return 1;
	}
	if (cell->row && same_path(cell+rows_n, cell-1)) {
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
	for (i = 0; i < options_n; i++) {
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
	for (i = 0; i < options_n; i++) {
		unchain->options[i] = options[i];
	}
	stack_unchains_n++;
}

void perform_chain(chain_t *chain) {
unsigned long i;
	for (i = 0; i < chain->options_n; i++) {
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
	link->links[--link->links_n] = NULL;
	link->links_max++;
	cell->links[--cell->links_n] = NULL;
	cell->links_max++;
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
	cell->links_max--;
	cell->links[cell->links_n++] = link;
	link->links_max--;
	link->links[link->links_n++] = cell;
}

void perform_unchain(chain_t *unchain) {
unsigned long i;
	unchain_cell(unchain->cell);
	for (i = 0; i < unchain->options_n; i++) {
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
cell_t *last, *cell = start, *next = next_cell(cell, NULL);
	cell->path = path;
	while (next) {
		last = cell;
		cell = next;
		next = next_cell(cell, last);
		cell->path = path;
	}
}

cell_t *next_cell(cell_t *cell, cell_t *last) {
unsigned long i;
	for (i = 0; i < cell->links_n && cell->links[i] == last; i++);
	if (i < cell->links_n) {
		return cell->links[i];
	}
	else {
		return NULL;
	}
}

void reset_color(color_t *color) {
	color->start->color = NULL;
	color->start->links_max++;
	color->end->color = NULL;
	color->end->links_max++;
}
