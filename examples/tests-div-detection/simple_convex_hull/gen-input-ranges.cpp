#include <iostream>
#include <fstream> 
#include <assert.h>
#include <stdlib.h>


using namespace std; 

float SecStart(float rmin, float rmax, int rdim, int sid) {
  assert(rdim > 0); 
  assert((0 <= sid) && (sid <= rdim)); 
  assert(rmax > rmin); 
  
  return rmin + (((rmax - rmin) / (float) rdim) * (float) sid); 
}


int main (int argc, char *argv[]) {
  if (argc != 8) {
    cerr << "Usage: ?/gen-input-ranges (range-file-name) (x dimension) (y dimension) (min-x-value) (max-x-value) (min-y-value) (max-y-value)" << endl;
    return -1; 
  }

  char *outname = argv[1]; 
  int xdim = atoi(argv[2]); 
  int ydim = atoi(argv[3]);
  float minx = atof(argv[4]);
  float maxx = atof(argv[5]);
  float miny = atof(argv[6]);
  float maxy = atof(argv[7]); 
  
  assert(xdim > 0 && ydim > 0);
  assert(minx < maxx); 
  assert(miny < maxy); 

  ofstream outfile(outname); 
  assert(outfile.good()); 

  for (int x = 0 ; x < xdim ; x++) {
    for (int y = 0 ; y < ydim ; y++) {
      outfile << SecStart(minx, maxx, xdim, x) << " " << SecStart(minx, maxx, xdim, (x+1)) << endl;
      outfile << SecStart(miny, maxy, ydim, y) << " " << SecStart(miny, maxy, ydim, (y+1)) << endl;
    }
  }
  
  outfile.close(); 
  
  return 0; 
}
