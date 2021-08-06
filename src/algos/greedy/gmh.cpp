#include "gmh.hpp"

using namespace std;


/*
 * Class constructor
 */
GMH::GMH(p_data* pd_) {
  int* tmp_ptr;

  // save ptr to problem data
  pd = pd_;

  // set seed for random as current time
  srand(time(0));

  // create and init umpire assignment matrix (n_slots, n_ump) (elem is visited venue / game)
  ump_assign = new int*[pd->n_slots];
  for (int i = 0; i < pd->n_slots; i++) {
    tmp_ptr = new int[pd->n_ump];
    memset(tmp_ptr, 0, sizeof(int) * pd->n_ump);
    ump_assign[i] = tmp_ptr;
  }
}


/*
 * main engine for greedy algorithm
 */
int** GMH::solve() {
  int slot = 1;
  int tmp_score;
  slot_s** slot_list;
  Score score(pd);

  // start with random ump assignment for slot 0
  random_assignment(ump_assign[0], pd->n_ump);

  // create slot list and it's assign objects
  slot_list = new slot_s*[pd->n_slots];
  for (int i = 0; i < pd->n_slots; i++) {
    slot_list[i] = new slot_s;
    slot_list[i]->assign[0] = create_assign(Score::big_num, pd->n_ump);
    slot_list[i]->assign[1] = create_assign(Score::big_num, pd->n_ump);
  }
  // mark first second-best assignment as visited already (edge case)
  slot_list[0]->assign[1]->visited = true;

  // iterate over all slots
  while (slot < pd->n_slots){
    // get home teams for this slot and sort them (needed by next_permutation)
    get_home_teams(ump_assign[slot], slot);
    sort(ump_assign[slot], ump_assign[slot] + pd->n_ump);

    do {
      // get score for this round and save best 2 ones (minimization)
      tmp_score = score.get_slot_score(ump_assign, slot);
      save_if_best(slot_list[slot], slot, tmp_score);

      // iterate over all permutations of umpires and games
    } while (next_permutation(ump_assign[slot], ump_assign[slot] + pd->n_ump));

    if (score.is_feasible(ump_assign, slot_list[slot]->assign[0]->sequence, slot) || slot_list[slot-1]->assign[1]->visited) {
      memcpy(ump_assign[slot], slot_list[slot]->assign[0]->sequence, sizeof(int) * pd->n_ump);
      slot_list[slot]->assign[0]->visited = true;
      slot++;
    } else {
      // if there's no feasible assignment on this slot then backtrack to previous slot
      memcpy(ump_assign[slot-1], slot_list[slot-1]->assign[1]->sequence, sizeof(int) * pd->n_ump);
      slot_list[slot-1]->assign[1]->visited = true;
      slot_list[slot]->assign[0]->score = Score::big_num;
      slot_list[slot]->assign[1]->score = Score::big_num;
    }
  }

  return ump_assign;
}


/*
 * get initial, random umpire - team assignment
 */
void GMH::random_assignment(int* int_arr, int n_ump) {
  get_home_teams(int_arr, 0);
  random_shuffle(int_arr, int_arr + n_ump);
}


/*
 * get home teams for a given slot and save into array
 */
void GMH::get_home_teams(int* int_arr, int slot) {
  int ump = 0;

  for (int team = 0; team < pd->n_teams; team++) {
    if (pd->opponents[slot][team] > 0) {
      int_arr[ump] = team;
      ump++;
    }
  }
}


/*
 * save best min score and 2nd best min score if applicable
 */
void GMH::save_if_best(slot_s* s, int slot, int check_score) {
  if (check_score < s->assign[0]->score) {
    s->assign[1]->score = s->assign[0]->score;
    s->assign[0]->score = check_score;

    // save assignment that has this score
    memcpy(s->assign[1]->sequence, s->assign[0]->sequence, sizeof(int) * pd->n_ump);
    memcpy(s->assign[0]->sequence, ump_assign[slot], sizeof(int) * pd->n_ump);
  } else if (check_score < s->assign[1]->score) {
    s->assign[1]->score = check_score;
    memcpy(s->assign[1]->sequence, ump_assign[slot], sizeof(int) * pd->n_ump);
  }
}
