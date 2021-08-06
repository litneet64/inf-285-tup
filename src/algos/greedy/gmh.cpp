#include "../../common.hpp"
#include "gmh.hpp"
#include "structs.hpp"

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
  // start with random ump assignment for slot 0
  random_assignment(ump_assign[0], pd->n_ump);
  int slot = 1;
  int tmp_score;
  slot_s** slot_list;

  // create slot list and it's assign objects
  slot_list = new slot_s*[pd->n_slots];
  for (int i = 0; i < pd->n_slots; i++) {
    slot_list[i] = new slot_s;
    slot_list[i]->assign[0] = create_assign(big_num, pd->n_ump);
    slot_list[i]->assign[1] = create_assign(big_num, pd->n_ump);
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
      tmp_score = get_slot_score(slot);
      save_if_best(slot_list[slot], slot, tmp_score);

      // iterate over all permutations of umpires and games
    } while (next_permutation(ump_assign[slot], ump_assign[slot] + pd->n_ump));

    if (is_feasible(slot_list[slot]->assign[0]->sequence, slot) || slot_list[slot-1]->assign[1]->visited) {
      memcpy(ump_assign[slot], slot_list[slot]->assign[0]->sequence, sizeof(int) * pd->n_ump);
      slot_list[slot]->assign[0]->visited = true;
      slot++;
    } else {
      // if there's no feasible assignment on this slot then backtrack to previous slot
      memcpy(ump_assign[slot-1], slot_list[slot-1]->assign[1]->sequence, sizeof(int) * pd->n_ump);
      slot_list[slot-1]->assign[1]->visited = true;
      slot_list[slot]->assign[0]->score = big_num;
      slot_list[slot]->assign[1]->score = big_num;
    }
  }

  return ump_assign;
}


/*
 * calc incentive as a positive int if a certain venue hasn't been visited before
 */
int GMH::incentive(int ump, int team, int slot) {
  int visits = 0;

  for (int s = 0; s < slot; s++) {
    if (ump_assign[s][ump] == team)
      visits++;
  }
  if (visits == 0)
    return incentive_bonus;

  return -1 * penalty * visits / 10;
}

/*
 * get distance from team k's venue to another team's venue
 */
int GMH::distance(int k_team, int team) {
  int dist = pd->distances[k_team][team];

  if (k_team == team)
    dist = big_num/10;

  return dist;
}

/*
 * calc total number of constraint violations that would be made with a given assigment
 */
int GMH::num_violations(int ump, int team, int slot) {
  int total_v = consecutive_home_visit_violations(ump, team, slot-1)
              + consecutive_team_seen_violations(ump, team, slot-1);

  return total_v;
}

/*
 * calc cost in order to guide greedy alg (we want to minimize this)
 */
int GMH::objective_cost(int ump, int team, int slot) {
  int k = ump_assign[slot-1][ump];
  int cost = distance(k, team) - incentive(ump, team, slot-1)
            + penalty * num_violations(ump, team, slot);

  return cost;
}


/*
 * check if given assignment is feasible or not
 */
bool GMH::is_feasible(int* assign, int slot) {
  for (int u = 0; u < pd->n_ump; u++) {
    if (num_violations(u, assign[u], slot) != 0)
      return false;
  }

  return true;
}


/*
 * get initial, random umpire - team assignment
 */
void GMH::random_assignment(int* int_arr, int n_ump) {
  get_home_teams(int_arr, 0);
  random_shuffle(int_arr, int_arr + n_ump);
}

/*
 * calc score for an entire assignment row
 */
int GMH::get_slot_score(int slot) {
  int score = 0;
  for (int u = 0; u < pd->n_ump; u++) {
    score += objective_cost(u, ump_assign[slot][u], slot);
  }

  return score;
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

/*
 * calc num of consecutive home visited home violations
 */
int GMH::consecutive_home_visit_violations(int ump, int new_team, int new_slot) {
  int cons_violations = 0;

  // traverse through the last q1 slots checking if we visited this venue already with given umpire
  for (int s = new_slot; s > (new_slot - pd->q1 + 1) && s > 0; s--) {
    if (ump_assign[s][ump] == new_team)
      cons_violations++;
  }

  return cons_violations;
}


/*
 * calc num of consecutive team seen violations
 */
int GMH::consecutive_team_seen_violations(int ump, int new_team, int new_slot) {
  int home_team, visit_team;
  int cons_violations = 0;

  // check that new team and it's opponent hasn't been seen before on the last q2 games by a given umpire
  for (int s = new_slot; s > (new_slot - pd->q2 + 1) && s > 0; s--) {
    home_team = ump_assign[s][ump];
    visit_team = pd->opponents[s][home_team] - 1;

    if (home_team == new_team || visit_team == new_team)
      cons_violations++;
  }

  return cons_violations;
}
