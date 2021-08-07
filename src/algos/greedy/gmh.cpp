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
 * main engine for probabilistic greedy algorithm
 */
int** GMH::solve() {
  int64_t tmp_score;
  int next_choice;
  slot_s** slot_list;
  Score score(pd);
  int slot = 1;

  // start with random ump assignment for slot 0
  random_assignment(ump_assign[0], pd->n_ump);

  // create slot list and init slot objects
  slot_list = new slot_s*[pd->n_slots];
  for (int i = 0; i < pd->n_slots; i++) {
    slot_list[i] = create_slot(pd->n_ump);
  }
  // mark this slot as backtracked already (edge case)
  slot_list[0]->backtracked = true;

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


    next_choice = probabilistic_next(&score, slot_list[slot], slot);

    // found at least one feasible choice and haven't backtracked
    if (next_choice != -1 && !slot_list[slot]->backtracked) {
      memcpy(ump_assign[slot], slot_list[slot]->assign[next_choice]->sequence, sizeof(int) * pd->n_ump);
      slot_list[slot]->free_choice = (next_choice + 1) % 2;
      slot++;
    } else {
      if (slot_list[slot]->backtracked) {
        // has backtracked already so don't mind and continue with any choice
        next_choice = rand() % 2;
        memcpy(ump_assign[slot], slot_list[slot]->assign[next_choice]->sequence, sizeof(int) * pd->n_ump);
        slot_list[slot]->free_choice = (next_choice + 1) % 2;
        slot++;
      } else {
        // no feasible assignment on this slot so backtrack to previous one with only free choice that's left
        memcpy(ump_assign[slot-1], slot_list[slot-1]->assign[slot_list[slot]->free_choice]->sequence, sizeof(int) * pd->n_ump);
        slot_list[slot-1]->backtracked = true;
        slot_list[slot]->assign[0]->score = Score::big_num;
        slot_list[slot]->assign[1]->score = Score::big_num;
      }
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
 * select next option with each one having 50/50 chance
 */
int GMH::probabilistic_next(Score* score, slot_s* s, int slot) {
  int next_choice;
  bool first_b = score->is_feasible(ump_assign, s->assign[0]->sequence, slot);
  bool sec_b = score->is_feasible(ump_assign, s->assign[1]->sequence, slot);

  // both are feasible assignments
  if (first_b && sec_b) {
    // choose randomly which path to continue, with equal probability for both choices
    next_choice = rand() % 2;
  } else if (first_b) {
    // first assignment is feasible only so keep that one
    next_choice = 0;
  } else if (sec_b) {
    // ditto for sec assignment
    next_choice = 1;
  } else {
    // no feasible assignment
    next_choice = -1;
  }

  return next_choice;
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
void GMH::save_if_best(slot_s* s, int slot, int64_t check_score) {
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
