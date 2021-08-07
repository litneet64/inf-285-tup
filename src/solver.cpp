#include "common.hpp"
#include "util/parser.hpp"
#include "util/debug.hpp"
#include "algos/greedy/gmh.hpp"
#include "algos/evol_alg/evol.hpp"
#include <chrono>

using namespace std;

int main(int argc, char *argv[]) {
    p_data pd;
    int** sol;

    // parse problem data from args and file
    parse_data(argc, argv, &pd);

    Score score(&pd);
    EA evol_algorithm(&pd);

    // call genetic algorithm, timing execution
    auto t_start = chrono::high_resolution_clock::now();
    sol = evol_algorithm.solve();
    auto t_end = chrono::high_resolution_clock::now();

    double elapsed_time_ms = chrono::duration<double, milli>(t_end-t_start).count();

    score.show_solution(sol, elapsed_time_ms);
}
