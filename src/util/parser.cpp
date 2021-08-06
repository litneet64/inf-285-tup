#include "../common.hpp"
#include "error_handler.hpp"
#include "parser.hpp"
using namespace std;

/*
 * Parses args and store those in the problem data struct
 */
void arg_parser(int argc, char *argv[], string* filename, p_data* pd) {
    if (argc != 5) {
        fatal_error(BAD_NUM_ARGS);
    }

    *filename = argv[1];

    // fill part of data struct
    pd->q1 = atoi(argv[2]);
    pd->q2 = atoi(argv[3]);
    pd->max_steps = atoi(argv[4]);

    return;
}

/*
 * Parses file name and file contents in order to fill remaining problem data struct
 */
void file_parser(string* f_name, p_data* pd) {
    regex filename_rgx(".*/umps([0-9]+).txt");
    regex int_rgx("(-[0-9]+|[0-9]+)");
    smatch matches;
    sregex_iterator iter, end;
    string line_buf;
    int **dist, **opp;

    int dist_i = 0, dist_j = 0;
    int opp_i = 0, opp_j = 0;
    string file_cont = "";

    if (!regex_search(*f_name, matches, filename_rgx)) {
        fatal_error(BAD_FILENAME);
    }

    // get team size, num of slots and umpires
    pd->n_teams = stoi(matches[1].str());
    pd->n_slots = 2 * pd->n_teams - 2;
    pd->n_ump = pd->n_teams / 2;

    // create distance and opponents array
    dist = new int* [pd->n_teams];
    opp = new int* [pd->n_slots];

    // create inner arrays for both matrices
    for (int i = 0; i < pd->n_teams; i++)
        dist[i] = new int[pd->n_teams];
        
    for (int i = 0; i < pd->n_slots; i++)
        opp[i] = new int[pd->n_teams];


    // get file contents line by line into a string
    fstream conf_file(*f_name);
    while (getline(conf_file, line_buf)) {
        file_cont += line_buf;
    }

    // match all ints and skip "nTeams"
    iter = sregex_iterator(file_cont.begin(), file_cont.end(), int_rgx);
    iter++;

    while (iter != end) {
      // fill opponents matrix
      if ((dist_i*dist_j) > ((pd->n_teams-1) * (pd->n_teams-1))) {
          if (opp_j == pd->n_teams) {
            opp_j = 0;
            opp_i++;
          }
          opp[opp_i][opp_j++] = stoi((*iter)[1]);
      } else {
          // fill distance matrix
          if (dist_j == pd->n_teams) {
            dist_j = 0;
            dist_i++;
          }
          dist[dist_i][dist_j++] = stoi((*iter)[1]);
      }
      ++iter;
    }

    // fill distance and opps on problem data struct
    pd->distances = dist;
    pd->opponents = opp;
}

/*
 * Fills the problem data into custom struct
 */
void parse_data(int argc, char *argv[], p_data* pd) {
    string filename;

    arg_parser(argc, argv, &filename, pd);
    file_parser(&filename, pd);
}
