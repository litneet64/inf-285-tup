#ifndef SCORE_H
#define SCORE_H

#include "../common.hpp"
#include "parser.hpp"

class Score {
  p_data* pd;
  const int penalty = 1000000;
  const int incentive_bonus = 10000;
  const int param_1 = 10;

  public:
    const static int big_num = 67108863;

    Score(p_data*);
    int incentive(int**, int, int, int);
    int distance(int, int);
    int num_violations(int**, int, int, int);
    int get_slot_score(int**, int);
    int objective_cost(int**, int, int, int);
    int consecutive_home_visit_violations(int**, int, int, int);
    int consecutive_team_seen_violations(int**, int, int, int);
    bool is_feasible(int**, int*, int);
};

#endif
