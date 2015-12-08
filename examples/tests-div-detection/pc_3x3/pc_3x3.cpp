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

  WFT *matrix = (WFT*) malloc(sizeof(WFT) * 3 * 3); 

  matrix[0] = ((WFT) ax - (WFT) dx);
  matrix[1] = ((WFT) ay - (WFT) dy);
  matrix[2] = (matrix[0] * matrix[0]) + (matrix[1] * matrix[1]); 

  matrix[3] = ((WFT) bx - (WFT) dx);
  matrix[4] = ((WFT) by - (WFT) dy);
  matrix[5] = (matrix[3] * matrix[3]) + (matrix[4] * matrix[4]); 

  matrix[6] = ((WFT) cx - (WFT) dx);
  matrix[7] = ((WFT) cy - (WFT) dy);
  matrix[8] = (matrix[6] * matrix[6]) + (matrix[7] * matrix[7]); 

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

  int sign = (det >= 0.0 ? 1.0 : -1.0); 

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
