#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <assert.h>

using namespace std;


int main (int argc, char *argv[]) {
  assert(argc == 3);
  unsigned int n_inputs = atoi(argv[1]);
  char *ofname = argv[2];
  unsigned int n_inputs_0 = 0;
  unsigned int n_inputs_1 = 0;
  assert(n_inputs % 5 == 0 || n_inputs == 9);
  if (n_inputs != 9) {
    n_inputs_0 = (n_inputs / 5) * 3;
    n_inputs_1 = (n_inputs / 5) * 2;
  }
  else {
    n_inputs_0 = 6;
    n_inputs_1 = 3;
  }

  ofstream ofile (ofname); 

  for (unsigned int i = 0 ; i < n_inputs_0 ; i++) {
    ofile << "0.0 1.0" << endl;
  }
  for (unsigned int i = 0 ; i < n_inputs_1 ; i++) {
    ofile << "-1.0 1.0" << endl;
  }

  ofile.close();

  return 0;
}
