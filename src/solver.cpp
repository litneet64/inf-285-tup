#include "common.hpp"
#include "util/parser.hpp"
#include "util/debug.hpp"
#include "algos/greedy/gmh.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    p_data pd;
    int** initial_sol;

    // parse problem data from args and file
    parse_data(argc, argv, &pd);

    // get a feasible solution with a greedy heuristic
    GMH greedy_heuristic(&pd);
    initial_sol = greedy_heuristic.solve();

    output_solution(initial_sol, pd.n_slots, pd.n_ump);

    // use sol and call genetic algorithm

}
