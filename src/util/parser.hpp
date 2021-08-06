#ifndef PARSERS
#define PARSERS


struct p_data {
  int q1;
  int q2;
  int max_steps;
  int n_teams;
  int n_slots;
  int n_ump;
  int** distances;
  int** opponents;
};


void arg_parser(int, char *[], std::string *, p_data*);
void file_parser(std::string, p_data*);
void parse_data(int, char *[], p_data*);


#endif
