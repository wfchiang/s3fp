#include <stdio.h>
#include <assert.h>
#include "div_geometric_primitives.h"
extern "C" {
#include "quadmath.h"
} 

#ifndef IFT 
#define IFT float 
#endif 


int main (int argc, char *argv[]) {
  assert(sizeof(WFT) >= sizeof(IFT));

  assert(argc == 3); 
  char *inname = argv[1]; 
  char *outname = argv[2]; 
  assert(inname != NULL && outname != NULL); 
  FILE *infile = fopen(inname, "r"); 
  FILE *outfile = fopen(outname, "w"); 
  assert(infile != NULL && outfile != NULL); 

  IFT ax, ay, az;
  IFT bx, by, bz;
  IFT cx, cy, cz;
  IFT dx, dy, dz;

  fseek(infile, 0, SEEK_END); 
  long fsize = ftell(infile); 
  assert(fsize == (12 * sizeof(IFT)));
  fseek(infile, 0, SEEK_SET); 

  fread(&ax, sizeof(IFT), 1, infile); 
  fread(&ay, sizeof(IFT), 1, infile); 
  fread(&az, sizeof(IFT), 1, infile); 

  fread(&bx, sizeof(IFT), 1, infile); 
  fread(&by, sizeof(IFT), 1, infile); 
  fread(&bz, sizeof(IFT), 1, infile); 

  fread(&cx, sizeof(IFT), 1, infile); 
  fread(&cy, sizeof(IFT), 1, infile); 
  fread(&cz, sizeof(IFT), 1, infile); 

  fread(&dx, sizeof(IFT), 1, infile); 
  fread(&dy, sizeof(IFT), 1, infile); 
  fread(&dz, sizeof(IFT), 1, infile); 

  WFT *matrix = (WFT*) malloc(sizeof(WFT) * 3 * 3); 

  matrix[0] = ((WFT) ax) - ((WFT) dx);
  matrix[1] = ((WFT) ay) - ((WFT) dy);
  matrix[2] = ((WFT) az) - ((WFT) dz);

  matrix[3] = ((WFT) bx) - ((WFT) dx);
  matrix[4] = ((WFT) by) - ((WFT) dy);
  matrix[5] = ((WFT) bz) - ((WFT) dz);

  matrix[6] = ((WFT) cx) - ((WFT) dx);
  matrix[7] = ((WFT) cy) - ((WFT) dy);
  matrix[8] = ((WFT) cz) - ((WFT) dz);

  WFT det = 0.0; 

  switch (sizeof(WFT) * 8) {
  case 32:
    det = matrix_determinant_32(3, matrix); 
    break;
  case 64:
    det = matrix_determinant_64(3, matrix); 
    break;
  case 128:
    det = matrix_determinant_128(3, matrix); 
    break;
  default:
    assert(false); 
    break; 
  }

  // int sign = (det == 0.0 ? 0 : (det > 0.0 ? 1 : -1));
  int sign = (det >= 0.0 ? 1 : -1); 

  __float128 det128 = det; 
  __float128 sign128 = sign; 

  fwrite(&det128, sizeof(__float128), 1, outfile); 
  fwrite(&sign128, sizeof(__float128), 1, outfile); 

  fclose(infile); 
  fclose(outfile); 

  return 0; 
}
