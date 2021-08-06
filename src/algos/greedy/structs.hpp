#ifndef GMH_STRUCTS_H
#define GMH_STRUCTS_H

/*
* define assignment structure: saves current score, umpires and home venues to visit along with if this assignment was made already
*/
struct assignment {
  int score;
  int* sequence;
  bool visited;
};

/*
 * define slot structure, saving best assignment and second best one
 */
struct slot_s {
  assignment* assign[2];
};

assignment* create_assign(int, int);

#endif
