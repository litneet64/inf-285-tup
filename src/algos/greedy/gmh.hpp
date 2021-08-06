#ifndef GMH_H
#define GMH_H

#include "../../util/parser.hpp"
#include "structs.hpp"

class GMH {
    p_data* pd;
    int** ump_assign;
    const int penalty = 1000000;
    const int incentive_bonus = 10000;
    const int big_num = 67108863;

  public:
    //GMH();
    GMH(p_data*);
    int** solve();
    int incentive(int, int, int);
    int distance(int, int);
    int num_violations(int, int, int);
    int objective_cost(int, int, int);
    bool is_feasible(int*, int);
    void random_assignment(int*, int);
    int get_slot_score(int);
    void get_home_teams(int*, int);
    void save_if_best(slot_s*, int, int);
    int consecutive_home_visit_violations(int, int, int);
    int consecutive_team_seen_violations(int, int, int);
};

#endif
