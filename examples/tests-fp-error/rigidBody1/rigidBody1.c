#include <stdio.h> 
#include <assert.h> 
#include <quadmath.h> 


#define IFT float 
#define OFT __float128 



int main (int argc, char *argv[]) {
  assert(argc == 3); 
  char *iname = argv[1]; 
  char *oname = argv[2]; 
  
  FILE *ifile = fopen(iname, "r"); 
  FILE *ofile = fopen(oname, "w"); 
  assert(ifile != NULL); 

  IFT in_x1, in_x2, in_x3; 

  FT x1, x2, x3, r1; 

  fread(&in_x1, sizeof(IFT), 1, ifile); 
  fread(&in_x2, sizeof(IFT), 1, ifile); 
  fread(&in_x3, sizeof(IFT), 1, ifile); 

  x1 = in_x1; 
  x2 = in_x2; 
  x3 = in_x3; 

  r1 = -1.0 * x1 * x2 - 
    2.0 * x2 * x3 - 
    x1 - 
    x3; 

  OFT out_r1 = r1; 
  fwrite(&out_r1, sizeof(OFT), 1, ofile); 

  fclose(ifile); 
  fclose(ofile); 
  

  return 0; 
}
