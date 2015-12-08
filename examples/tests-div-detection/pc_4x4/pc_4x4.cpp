#include <stdio.h>
#include <assert.h>
#include "div_geometric_primitives.h"
extern "C" {
#include "quadmath.h"
} 
#include "s3fp_utils.h"

#ifndef IFT
#define IFT float
#endif 

#ifndef OFT 
#define OFT __float128 
#endif 


int main (int argc, char *argv[]) {
  assert(argc == 3); 

  FILE *infile = s3fpGetInFile(argc, argv); 
  FILE *outfile = s3fpGetOutFile(argc, argv); 

  unsigned long fsize = s3fpFileSize(infile); 
  assert(fsize == 8 * sizeof(IFT)); 

  IFT ax, ay;
  IFT bx, by;
  IFT cx, cy;
  IFT dx, dy;

  fread(&ax, sizeof(IFT), 1, infile); 
  fread(&ay, sizeof(IFT), 1, infile); 

  fread(&bx, sizeof(IFT), 1, infile); 
  fread(&by, sizeof(IFT), 1, infile); 

  fread(&cx, sizeof(IFT), 1, infile); 
  fread(&cy, sizeof(IFT), 1, infile); 

  fread(&dx, sizeof(IFT), 1, infile); 
  fread(&dy, sizeof(IFT), 1, infile); 

  WFT *matrix = (WFT*) malloc(sizeof(WFT) * 4 * 4); 

  matrix[0] = (WFT) ax; 
  matrix[1] = (WFT) ay;
  matrix[2] = (matrix[0] * matrix[0]) + (matrix[1] * matrix[1]); 
  matrix[3] = (WFT) 1.0; 

  matrix[4] = (WFT) bx; 
  matrix[5] = (WFT) by; 
  matrix[6] = (matrix[4] * matrix[4]) + (matrix[5] * matrix[5]); 
  matrix[7] = (WFT) 1.0; 

  matrix[8] = (WFT) cx; 
  matrix[9] = (WFT) cy; 
  matrix[10] = (matrix[8] * matrix[8]) + (matrix[9] * matrix[9]); 
  matrix[11] = (WFT) 1.0; 

  matrix[12] = (WFT) dx; 
  matrix[13] = (WFT) dy; 
  matrix[14] = (matrix[12] * matrix[12]) + (matrix[13] * matrix[13]); 
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

  int sign = ((det >= 0.0) ? 1.0 : -1.0); 

  OFT odata = 1; 
  fwrite(&odata, sizeof(OFT), 1, outfile); 
  fwrite(&odata, sizeof(OFT), 1, outfile); 

  OFT sign128 = sign; 
  fwrite(&odata, sizeof(OFT), 1, outfile); 
  fwrite(&sign128, sizeof(OFT), 1, outfile); 

  fclose(infile); 
  fclose(outfile); 

  return 0; 
}
