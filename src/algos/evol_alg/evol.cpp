#include "evol.hpp"

using namespace std;

EA::EA(p_data* pd_) {
  int** tmp_ptr;

  pd = pd_;
  n_gens = pd->max_steps;


  // save space in heap for current best sol only
  curr_best_sol = new solution;
  tmp_ptr = new int*[pd->n_slots];
  for (int s = 0; s < pd->n_slots; s++)
    tmp_ptr[s] = new int[pd->n_ump];

  curr_best_sol->sol_arr = tmp_ptr;
  curr_best_sol->score = Score::big_num;
}

/*
 * Main evolutive algorithm engine
 */
int** EA::solve() {
  int par_1, par_2, cross_slot;
  solution_set tmp_sols, current_sols;
  Score score(pd);
  int sol_c = 0, gen = 0;

  // START OF UNNECESARY PREFACE
  // init child sol in heap
  int** child_sol = new int*[pd->n_slots];
  for (int s = 0; s < pd->n_slots; s++)
    child_sol[s] = new int[pd->n_ump];

  // let's try to keep solutions' matrices in the stack for perfomance's sake
  int** tmp_1[n_sols/2];
  int** c_1[n_sols];
  int** c_2[n_sols];

  int* tmp_1_1[n_sols/2][pd->n_slots];
  int* c_1_1[n_sols][pd->n_slots];
  int* c_2_1[n_sols][pd->n_slots];

  // init int matrices with 0s
  int tmp_1_1_1[n_sols/2][pd->n_slots][pd->n_ump] = {0};
  int c_1_1_1[n_sols][pd->n_slots][pd->n_ump] = {0};
  int c_2_1_1[n_sols][pd->n_slots][pd->n_ump] = {0};

  // assign pointers to initialized int arrays on previous int* arrays
  for (int solc = 0; solc < n_sols/2; solc++) {
    for (int slot = 0; slot < pd->n_slots; slot++) {
      tmp_1_1[solc][slot] = &tmp_1_1_1[solc][slot][0];
    }
    tmp_1[solc] = &tmp_1_1[solc][0];
  }

  // assign pointers to initialized int arrays on previous int* arrays
  for (int solc = 0; solc < n_sols; solc++) {
    for (int slot = 0; slot < pd->n_slots; slot++) {
      c_1_1[solc][slot] = &c_1_1_1[solc][slot][0];
      c_2_1[solc][slot] = &c_2_1_1[solc][slot][0];
    }
    c_1[solc] = &c_1_1[solc][0];
    c_2[solc] = &c_2_1[solc][0];
  }

  solution* tmp_s1[n_sols/2];
  solution* c_s1[n_sols];
  solution* c_s2[n_sols];

  // pass zero arrays' addresses to sol_arr attr in solution list
  for (int i = 0; i < n_sols/2; i++) {
    tmp_s1[i] = new solution;
    tmp_s1[i]->sol_arr = tmp_1[i];
  }
  for (int i = 0; i < n_sols; i++) {
    c_s1[i] = new solution;
    c_s2[i] = new solution;
    c_s1[i]->sol_arr = c_1[i];
    c_s2[i]->sol_arr = c_2[i];
  }

  // pass solutions list to main solution set struct
  tmp_sols.t = &tmp_s1[0];
  current_sols.t = &c_s1[0];
  current_sols.t_1 = &c_s2[0];
  // END OF UNNECESARY PREFACE


  // EVOLUTIVE ALGORITHM STARTS HERE
  // get initial solutions using prob. greedy heuristic
  initial_sols(current_sols.t);

  while (gen < n_gens) {
    while (sol_c < n_sols/2) {
      // get 2 random parents
      par_1 = rand() % n_sols;
      par_2 = rand() % n_sols;

      if (par_1 == par_2) continue;

      // select random crossover point and make crossover
      cross_slot = rand() % pd->n_slots;
      crossover(current_sols.t[par_1], current_sols.t[par_2], child_sol, cross_slot);

      // child not already found in tmp_sols(time) or current_sols(time-1)
      if (sol_inside(tmp_sols.t, child_sol, n_sols/2) || sol_inside(current_sols.t_1, child_sol, n_sols)) {
        continue;
      }
      save_sol(tmp_sols.t[sol_c], child_sol);

      sol_c++;
    }

    // calc scores for every sol in current_sols(time)
    eval_scores(current_sols.t, &score, n_sols);
    eval_scores(tmp_sols.t, &score, n_sols/2);

    // sort current_sols(time) in order to replace the last n_sols / 2 solutions with individuals from tmp_sols(time)
    sort(current_sols.t, current_sols.t + n_sols, &sol_comparator);
    sort(tmp_sols.t, tmp_sols.t + n_sols/2, &sol_comparator);
    copy_solutions(current_sols.t + n_sols/2 - 1, tmp_sols.t, n_child);
    copy_solutions(current_sols.t_1, current_sols.t, n_sols);

    // assign scores according to total traveled distance and save best sol if pertinent
    dist_scores(current_sols.t, &score);
    get_best_sol(current_sols.t);

    // apply mutation operator on current_sols(time-1)
    mutation(current_sols.t);


    gen++;
  }

  return curr_best_sol->sol_arr;
}


/*
 * Assign scores to solution list according to evaluation function
 */
void EA::eval_scores(solution** sol_ls, Score* score, int total_sols) {
  int64_t tmp_score;

  for (int sol_c = 0; sol_c < total_sols; sol_c++) {
    tmp_score = score->score_evaluation(sol_ls[sol_c]->sol_arr);
    sol_ls[sol_c]->score = tmp_score;
  }
}


/*
 * Assign scores to solution list according to total distance traveled
 */
void EA::dist_scores(solution** sol_ls, Score* score) {
  int64_t tmp_score;

  for (int sol_c = 0; sol_c < n_sols; sol_c++) {
    tmp_score = score->score_distance(sol_ls[sol_c]->sol_arr);
    sol_ls[sol_c]->score = tmp_score;
  }
}


/*
 * Get N initial sols using probabilistic greedy heuristic
 */
void EA::initial_sols(solution** sol_ls) {
  GMH greedy_alg(pd);
  int** tmp_ptr;

  // fill array with sols found by probabilistic greedy heuristic
  for (int sol = 0; sol < n_sols; sol++) {
    sol_ls[sol]->score = Score::big_num;
    tmp_ptr = greedy_alg.solve();
    save_sol(sol_ls[sol], tmp_ptr);
  }

}


/*
 * apply crossover to 2 parents on a given slot and write child solution
 */
void EA::crossover(solution* par_1, solution* par_2, int** child, int slot_t) {
  // copy first parent's genetic material from start to given slot
  for (int t = 0; t < slot_t; t++) {
    for (int u = 0; u < pd->n_ump; u++) {
      child[t][u] = par_1->sol_arr[t][u];
    }
  }

  // copy second parent's genetic material from slot_t to end
  for (int t = slot_t; t < pd->n_slots; t++) {
    for (int u = 0; u < pd->n_ump; u++) {
      child[t][u] = par_2->sol_arr[t][u];
    }
  }
}


/*
 * get best solution from list and save it on class' private attr
 */
void EA::get_best_sol(solution** sol_list) {
  solution* best_sol = new solution;
  best_sol->score = Score::big_num;

  for (int sol_c = 0; sol_c < n_sols; sol_c++)
    if (sol_list[sol_c]->score < best_sol->score)
      best_sol = sol_list[sol_c];

  // save best found if better than current best
  if (best_sol->score < curr_best_sol->score) {
    save_sol(curr_best_sol, best_sol->sol_arr);
    curr_best_sol->score = best_sol->score;
  }
}


/*
 * Save solution (as an int matrix) in solution struct array
 */
void EA::save_sol(solution* sol, int** child) {
  // copy child solution to solution list
  for (int s = 0; s < pd->n_slots; s++)
    for (int u = 0; u < pd->n_ump; u++)
      sol->sol_arr[s][u] = child[s][u];
}


/*
 * copy n solutions from sol_ls_2 to sol_ls_1
 */
void EA::copy_solutions(solution** sol_ls_1, solution** sol_ls_2, int sols) {
  for (int sol_c = 0; sol_c < sols; sol_c++) {
    // save sol number sol_c in sol_ls_1 elem by elem, then save score too
    save_sol(sol_ls_1[sol_c],  sol_ls_2[sol_c]->sol_arr);
    sol_ls_1[sol_c]->score = sol_ls_2[sol_c]->score;
  }
}


/*
 * Checks if solution is inside our list
 */
bool EA::sol_inside(solution** sol_ls, int** child, int total_sols) {
  bool same_sol;

  for (int sol_c = 0; sol_c < total_sols; sol_c++) {
    same_sol = true;
    for (int s = 0; s < pd->n_slots; s++) {
      for (int u = 0; u < pd->n_ump; u++) {
        if (sol_ls[sol_c]->sol_arr[s][u] != child[s][u]){
          same_sol = false;
          break;
        }
      }
      if (!same_sol) break;
    }
    if (same_sol) return true;
  }

  return false;
}


/*
 * custom comparator for solution struct objects
 */
bool sol_comparator(solution* s1, solution* s2) {
  return (s1->score < s2->score);
}


/*
 * Probabilistic mutator for a given solution list
 */
void EA::mutation(solution** sol_ls) {
  int rand_ind;

  // calc number of individuals that will mutate with mutation probability
  int n_mutations = (int) n_sols * mutation_p;

  while (n_mutations > 0) {
    // apply mutation operator to random individual
    rand_ind = rand() % n_sols;
    mutation_operator(sol_ls[rand_ind]);

    n_mutations--;
  }
}


/*
 * Mutation operator for a given individual, swaps the assigned home venues for 2 random umpires in a random slot
 */
void EA::mutation_operator(solution* individual) {
  int tmp_team;
  int ump_1, ump_2;

  // get random slot
  int slot = rand() % pd->n_slots;

  // get random umpires to swap assigned teams
  ump_1 = rand() % pd->n_ump;
  ump_2 = rand() % pd->n_ump;

  // get new ump if they are the same one
  while (ump_1 == ump_2)
    ump_2 = rand() % pd->n_ump;

  // swap home venues
  tmp_team = individual->sol_arr[slot][ump_1];
  individual->sol_arr[slot][ump_1] = individual->sol_arr[slot][ump_2];
  individual->sol_arr[slot][ump_2] = tmp_team;
}
