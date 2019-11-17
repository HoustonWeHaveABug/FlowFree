# FlowFree
FlowFree puzzles solver and generator written in C, works also for Numberlink/Arukone puzzles.

Both solver and generator are implemented in the same source code (flowfree.c) and executable. Only input differs depending on which functionality you want to use.

A format converter is also available ("Raetsel" converter, see below).

The makefiles provided to generate the executables work only on Linux family operating systems but the sources are not OS specific.

## The solver

The solver is a solution to challenge https://www.reddit.com/r/dailyprogrammer/comments/4zog32/20160826_challenge_280_hard_free_flow_solver/, please read this page for more details especially regarding the input format specifications.

The basic idea is at each step to choose the cell that has the least valid choices to link with its neighbours, then lock this cell and the links chosen, and go to next step until a solution is found (all cells are exhausted) or an inconsistency is encountered (a cell has no valid choices). If a cell with one choice is encountered it is immediately locked without searching for another one.

The search is iterative and is using explicit stacks, to be able to solve large puzzles without having issue with stack size overflow when the search is done recursively.

First it tries to solve the puzzle with the additional constraint that no paths between two endpoints are self-touching, it means that one cell cannot have more that 2 neighbours of the same color (1 for an endpoint).

This additional constraint reduces the search space tremendously and allows to solve a lot of large grids almost instantly.

If the puzzle cannot be solved this way it tries again without this constraint. It seems that NumberLink/Arukone puzzles forbid self-touching implicitly, while FlowFree puzzles may allow it.

The solver displays the first solution found and at the end of execution the number of nodes (search space size) and the total number of solutions found.

## The generator

The generator was developed to test the solver, it generates random starting positions and calls the solver until one position is solved with the criteria provided in input.

Sample input data for the generator:

12 16 14 0 3 1 1000000

It means that the generator must create a puzzle with 12 colors in a grid of 16 columns by 14 rows, with self-touching forbidden, and a minimal distance of 3 cells between two endpoints of the same color. The puzzle shall not have more than 1 solution. The number of attempts made is shown every 1000000 grids.

If a grid is solved successfully but with more solutions than requested in input it is displayed with the message "Too many solutions" below.

## Test data

The Puzzles folder contains grids from various sources converted to the solver input format:

| Files | Source |
| ------------------------------ | ------------------------------------------------------------------------------------------------------------ |
| flowfree_\*\_random\*.txt | Created by the generator |
| flowfree_example_\*.txt | https://www.reddit.com/r/dailyprogrammer/comments/4zog32/20160826_challenge_280_hard_free_flow_solver/ |
| flowfree_huge/large/killer.txt | https://github.com/thomasahle/numberlink/tree/master/puzzles |
| flowfree_nikoli_\*.txt | http://www.nikoli.com/en/event/puzzle_hayatoki.html |
| flowfree_oxford_\*.txt | http://spivey.oriel.ox.ac.uk/wiki/index.php/Programming_competition_results |
| flowfree_raetsel_\*.txt | http://www.janko.at/Raetsel/Arukone/ |
| flowfree_wikipedia.txt | https://en.wikipedia.org/wiki/Numberlink |

The others are FlowFree official grids.

## The "Raetsel" converter

convert_raetsel.c converts grids from the format used by this numberlink solver in Copris: http://bach.istc.kobe-u.ac.jp/copris/puzzles/numberlink/ (which is using grids from http://www.janko.at/Raetsel/Arukone/) to the format used by the FlowFree solver.

Input example
```
7
12
12
- - - - - - - - - - - 1
- 2 7 - - - - - - - - -
- - - - - - - - - - - -
- - - - - - - - - 4 - -
3 - - 6 - - - - - - - -
- - - - - 6 - - - - - -
- - - - - - - 3 - - - -
- - - - 5 - - - - - - -
- - - - - - - - - - - -
- - 4 - - - - - - - 2 -
- - - - - - - - - - 5 -
- - - - - 1 - - - 7 - -
```
Output
```
7 12 12
(11, 0) (5, 11)
(1, 1) (10, 9)
(0, 4) (7, 6)
(9, 3) (2, 9)
(4, 7) (10, 10)
(3, 4) (5, 5)
(2, 1) (9, 11)
```
