#ifndef GMH_STRUCTS_H
#define GMH_STRUCTS_H

#include "../../util/score.hpp"

/*
* define assignment structure: saves current score, umpires and home venues to visit along with if this assignment was made already
*/
struct assignment {
  int64_t score;
  int* sequence;
};

/*
 * define slot structure, saving best assignment and second best one
 */
struct slot_s {
  assignment* assign[2];
  bool backtracked;
  int free_choice;
};

assignment* create_assign(int);
slot_s* create_slot(int);

#endif
