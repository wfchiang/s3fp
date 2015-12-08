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

  WFT *matrix = (WFT*) malloc(sizeof(WFT) * 4 * 4); 

  matrix[0] = (WFT) ax;
  matrix[1] = (WFT) ay;
  matrix[2] = (WFT) az;
  matrix[3] = (WFT) 1.0; 

  matrix[4] = (WFT) bx;
  matrix[5] = (WFT) by;
  matrix[6] = (WFT) bz;
  matrix[7] = (WFT) 1.0; 

  matrix[8] = (WFT) cx;
  matrix[9] = (WFT) cy;
  matrix[10] = (WFT) cz;
  matrix[11] = (WFT) 1.0; 

  matrix[12] = (WFT) dx;
  matrix[13] = (WFT) dy;
  matrix[14] = (WFT) dz;
  matrix[15] = (WFT) 1.0; 

  WFT det = 0.0; 

  switch (sizeof(WFT) * 8) {
  case 32:
    det = matrix_determinant_32(4, matrix); 
    break;
  case 64:
    det = matrix_determinant_64(4, matrix); 
    break;
  case 128:
    det = matrix_determinant_128(4, matrix); 
    break;
  default:
    assert(false); 
    break; 
  }

  // int sign = (det == 0.0 ? 0 : (det > 0.0 ? 1 : -1));
  int sign = (det >= 0.0 ? 1 : -1); 

  __float128 odata = 1; 
  fwrite(&odata, sizeof(__float128), 1, outfile); 
  fwrite(&odata, sizeof(__float128), 1, outfile); 

  __float128 sign128 = sign; 
  fwrite(&odata, sizeof(__float128), 1, outfile); 
  fwrite(&sign128, sizeof(__float128), 1, outfile); 

  fclose(infile); 
  fclose(outfile); 

  return 0; 
}
