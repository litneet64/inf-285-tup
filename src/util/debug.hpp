#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include "../algos/evol_alg/structs.hpp"
#include "score.hpp"

void output_solution(int**, int, int);
void output_sol_matrix(int**, int, int, int, int);
void output_solution_set(solution**, int, int, int);
void output_best_in_gen(int**, Score*, int);
void output_distance_costs_in_gen(solution**, Score*, int, int);
void output_eval_costs_in_gen(solution**, Score*, int, int);

#endif
