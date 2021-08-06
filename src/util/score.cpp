#include "score.hpp"

/*
 * Scoring class constructor
 */
Score::Score(p_data* pd_) : pd(pd_) {}

/*
 * calc incentive as a positive int if a certain venue hasn't been visited before
 */
int Score::incentive(int** ump_assign, int ump, int team, int slot) {
  int visits = 0;

  for (int s = 0; s < slot; s++) {
    if (ump_assign[s][ump] == team)
      visits++;
  }
  if (visits == 0)
    return incentive_bonus;

  return -1 * penalty * visits / param_1;
}


/*
 * get distance from team k's venue to another team's venue
 */
int Score::distance(int k_team, int team) {
  int dist = pd->distances[k_team][team];

  if (k_team == team)
    dist = big_num / param_1;

  return dist;
}


/*
 * calc total number of constraint violations that would be made with a given assigment
 */
int Score::num_violations(int** ump_assign, int ump, int team, int slot) {
  int total_v = consecutive_home_visit_violations(ump_assign, ump, team, slot-1)
              + consecutive_team_seen_violations(ump_assign, ump, team, slot-1);

  return total_v;
}


/*
 * calc score for an entire assignment row
 */
int Score::get_slot_score(int** ump_assign, int slot) {
  int score = 0;
  for (int u = 0; u < pd->n_ump; u++) {
    score += objective_cost(ump_assign, u, ump_assign[slot][u], slot);
  }

  return score;
}


/*
 * calc cost in order to guide greedy alg (we want to minimize this)
 */
int Score::objective_cost(int** ump_assign, int ump, int team, int slot) {
  int k = ump_assign[slot-1][ump];
  int cost = distance(k, team) - incentive(ump_assign, ump, team, slot-1)
            + penalty * num_violations(ump_assign, ump, team, slot);

  return cost;
}


/*
 * check if given assignment is feasible or not
 */
bool Score::is_feasible(int** ump_assign, int* assign, int slot) {
  for (int u = 0; u < pd->n_ump; u++) {
    if (num_violations(ump_assign, u, assign[u], slot) != 0)
      return false;
  }

  return true;
}


/*
 * calc num of consecutive home visited home violations
 */
int Score::consecutive_home_visit_violations(int** ump_assign, int ump, int new_team, int new_slot) {
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
int Score::consecutive_team_seen_violations(int** ump_assign, int ump, int new_team, int new_slot) {
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
