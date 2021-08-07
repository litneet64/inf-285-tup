#include "structs.hpp"

/*
 * populate slot structure with values and return ptr to it
 */
assignment* create_assign(int n_ump) {
  assignment* a = new assignment;
  a->score = Score::big_num;
  a->sequence = new int[n_ump];

  return a;
}

slot_s* create_slot(int n_ump) {
  slot_s* s = new slot_s;
  s->assign[0] = create_assign(n_ump);
  s->assign[1] = create_assign(n_ump);
  s->backtracked = false;
  s->free_choice = -1;

  return s;
}
