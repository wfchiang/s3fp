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
  assert(fsize == 15 * sizeof(IFT)); 

  IFT ax, ay, az;
  IFT bx, by, bz;
  IFT cx, cy, cz;
  IFT dx, dy, dz;
  IFT ex, ey, ez; 

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

  fread(&ex, sizeof(IFT), 1, infile); 
  fread(&ey, sizeof(IFT), 1, infile); 
  fread(&ez, sizeof(IFT), 1, infile); 

  WFT *matrix = (WFT*) malloc(sizeof(WFT) * 4 * 4); 

  matrix[0] = ((WFT) ax - (WFT) ex);
  matrix[1] = ((WFT) ay - (WFT) ey);
  matrix[2] = ((WFT) az - (WFT) ez);
  matrix[3] = ((matrix[0] * matrix[0]) + (matrix[1] * matrix[1])) + (matrix[2] * matrix[2]);

  matrix[4] = ((WFT) bx - (WFT) ex);
  matrix[5] = ((WFT) by - (WFT) ey);
  matrix[6] = ((WFT) bz - (WFT) ez);
  matrix[7] = ((matrix[4] * matrix[4]) + (matrix[5] * matrix[5])) + (matrix[6] * matrix[6]);

  matrix[8] = ((WFT) cx - (WFT) ex);
  matrix[9] = ((WFT) cy - (WFT) ey);
  matrix[10] = ((WFT) cz - (WFT) ez);
  matrix[11] = ((matrix[8] * matrix[8]) + (matrix[9] * matrix[9])) + (matrix[10] * matrix[10]);

  matrix[12] = ((WFT) dx - (WFT) ex);
  matrix[13] = ((WFT) dy - (WFT) ey);
  matrix[14] = ((WFT) dz - (WFT) ez);
  matrix[15] = ((matrix[12] * matrix[12]) + (matrix[13] * matrix[13])) + (matrix[14] * matrix[14]);

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
