#include "structs.hpp"

/*
 * populate slot structure with values and return ptr to it
 */
assignment* create_assign(int score, int n_ump) {
  assignment* a = new assignment;
  a->score = score;
  a->sequence = new int[n_ump];
  a->visited = false;

  return a;
}
