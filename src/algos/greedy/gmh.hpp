#ifndef GMH_H
#define GMH_H

#include "../../common.hpp"
#include "../../util/parser.hpp"
#include "../../util/score.hpp"
#include "structs.hpp"

class GMH {
    p_data* pd;
    int** ump_assign;

  public:
    GMH(p_data*);
    int** solve();
    int probabilistic_next(Score*, slot_s*, int);
    bool is_feasible(int*, int);
    void random_assignment(int*, int);
    void get_home_teams(int*, int);
    void save_if_best(slot_s*, int, int64_t);
};

#endif
