#include <iostream>
#include "debug.hpp"

using namespace std;

/*
 * Output sequence of visits for every umpire in an array in the form (n_slots x n_ump) where elem is team venue visited
 */
void output_solution(int** solution, int slot, int ump) {
  cout << "Solution:\n";
  for (int u = 0; u < ump; u++) {
    cout << "[+] ump " << u << ": ";
    for (int s = 0; s < slot; s++) {
      cout << solution[s][u] + 1;
      if (s != (slot - 1)) cout << "->";
    }
    cout << "\n";
  }
}
