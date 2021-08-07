#ifndef SCORE_H
#define SCORE_H

#include "../common.hpp"
#include "parser.hpp"

class Score {
  const int penalty = 1000000;
  const int incentive_bonus = 10000;
  const int param_1 = 10;

  public:
    p_data* pd;
    const static int big_num = 67108863;

    Score(p_data*);
    int64_t score_evaluation(int**);
    int64_t score_distance(int**);
    void show_solution(int**, double);
    int incentive(int**, int, int, int);
    int distance(int, int);
    int num_violations(int**, int, int, int);
    int64_t get_slot_score(int**, int);
    int64_t objective_cost(int**, int, int, int);
    int consecutive_home_visit_violations(int**, int, int, int);
    int consecutive_team_seen_violations(int**, int, int, int);
    bool is_feasible(int**, int*, int);
};

#endif
