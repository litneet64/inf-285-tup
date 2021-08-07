#include "debug.hpp"

using namespace std;

/*
 * Output sequence of visits for every umpire in an array in the form (n_slots x n_ump) where elem is team venue visited
 */
void output_solution(int** solution, int slot, int ump) {
  cout << "Solution:\n";
  for (int u = 0; u < ump; u++) {
    cout << "[+] ump " << u << ": ";
    for (int s = 0; s < slot; s++) {
      cout << solution[s][u] + 1;
      if (s != (slot - 1)) cout << "->";
    }
    cout << "\n";
  }
}

/*
 * Output solution as a matrix
 */
void output_sol_matrix(int** sol, int slot, int ump, int sol_c, int total_sols) {
  cout << "Solution matrix (" << sol_c << "/" << total_sols << "):\n";
  for (int s = 0; s < slot; s++) {
    for (int u = 0; u < ump; u++) {
      cout << sol[s][u] << "\t";
    }
    cout << "\n";
  }
}

/*
 * Output solution set in matrix representation
 */
void output_solution_set(solution** sol_ls, int slot, int ump, int sols) {
  cout << "Solution set:\n";
  for (int s = 0; s < sols; s++) {
    output_sol_matrix(sol_ls[s]->sol_arr, slot, ump, s, sols);
    cout << "\n";
  }
}

/*
 * Output best solution from current generation
 */
void output_best_in_gen(int** solution, Score* score, int gen) {
  int64_t dist = score->score_distance(solution);

  cout << "Gen: " << gen << "\n";
  cout << "Total Distance Cost: " << dist << "\n";
  for (int u = 0; u < score->pd->n_ump; u++) {
    cout << "Umpire " << u << ": ";
    for (int s = 0; s < score->pd->n_slots; s++) {
      cout << solution[s][u] + 1;
      if (s != (score->pd->n_slots - 1)) cout << "-";
    }
    cout << "\n";
  }
  cout << "\n";
}


/*
 * Output all distance costs in a given generation
 */
void output_distance_costs_in_gen(solution** sol_ls, Score* score, int gen, int sols) {
  int64_t dist;

  cout << "Gen " << gen << ":\n";
  for (int s = 0; s < sols; s++) {
    dist = score->score_distance(sol_ls[s]->sol_arr);
    cout << "\t[+] Sol " << s << " cost: " << dist << "\n";
  }
  cout << "\n";
}

/*
 * Output all evaluation function costs in a given generation
 */
void output_eval_costs_in_gen(solution** sol_ls, Score* score, int gen, int sols) {
  int64_t dist;

  cout << "Gen " << gen << ":\n";
  for (int s = 0; s < sols; s++) {
    dist = score->score_evaluation(sol_ls[s]->sol_arr);
    cout << "\t[+] Sol " << s << " cost: " << dist << "\n";
  }
  cout << "\n";
}
