#ifndef EVOL_H
#define EVOL_H

#include "../../common.hpp"
#include "../../util/parser.hpp"
#include "../../util/score.hpp"
#include "../../util/debug.hpp"
#include "../greedy/gmh.hpp"
#include "structs.hpp"

bool sol_comparator(solution*, solution*);

class EA {
    p_data* pd;
    int n_gens;
    solution* curr_best_sol;
    const float mutation_p = 0.45;
    const int n_sols = 260;
    const int n_child = n_sols/2;

  public:
    EA(p_data*);
    int** solve();
    void eval_scores(solution**, Score*, int);
    void dist_scores(solution**, Score*);
    void initial_sols(solution**);
    void crossover(solution*, solution*, int**, int);
    void get_best_sol(solution**);
    void save_sol(solution*, int**);
    void copy_solutions(solution**, solution**, int);
    bool sol_inside(solution**, int**, int);
    void mutation(solution**);
    void mutation_operator(solution*);
};

#endif
