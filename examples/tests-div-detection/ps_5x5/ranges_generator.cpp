#include <iostream> 
#include <fstream>
#include <stdlib.h>
#include <assert.h>

using namespace std; 


int main (int argc, char *argv[]) {
  assert(argc >= 8); 

  char *outname = argv[1]; 

  int dim = atoi(argv[2]); 
  assert(dim == 2 || dim == 3); 

  int n_points = atoi(argv[3]); 
  assert(n_points > 0); 

  float xlb = atof(argv[4]); 
  float xub = atof(argv[5]); 
  assert(xlb < xub); 

  float ylb = atof(argv[6]); 
  float yub = atof(argv[7]); 
  assert(ylb < yub); 

  float zlb = 0.0; 
  float zub = 0.0; 
  if (dim == 3) {
    assert(argc == 10); 
    zlb = atof(argv[8]); 
    zub = atof(argv[9]); 
    assert(zlb < zub); 
  }
 
  ofstream outfile(outname); 

  for (unsigned int i = 0 ; i < n_points ; i++) {
    outfile << xlb << " " << xub << endl;
    outfile << ylb << " " << yub << endl;
    if(dim == 3) 
      outfile << zlb << " " << zub << endl;
  }

  outfile.close(); 

  return 0; 
}
