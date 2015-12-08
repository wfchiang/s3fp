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

  WFT *matrix = (WFT*) malloc(sizeof(WFT) * 5 * 5); 

  matrix[0] = (WFT) ax;
  matrix[1] = (WFT) ay;
  matrix[2] = (WFT) az;
  matrix[3] = (((WFT)ax*(WFT)ax) + ((WFT)ay*(WFT)ay)) + ((WFT)az*(WFT)az);
  matrix[4] = (WFT) 1.0; 

  matrix[5] = (WFT) bx;
  matrix[6] = (WFT) by;
  matrix[7] = (WFT) bz;
  matrix[8] = (((WFT)bx*(WFT)bx) + ((WFT)by*(WFT)by)) + ((WFT)bz*(WFT)bz);
  matrix[9] = (WFT) 1.0; 

  matrix[10] = (WFT) cx;
  matrix[11] = (WFT) cy;
  matrix[12] = (WFT) cz;
  matrix[13] = (((WFT)cx*(WFT)cx) + ((WFT)cy*(WFT)cy)) + ((WFT)cz*(WFT)cz);
  matrix[14] = (WFT) 1.0; 

  matrix[15] = (WFT) dx;
  matrix[16] = (WFT) dy;
  matrix[17] = (WFT) dz;
  matrix[18] = (((WFT)dx*(WFT)dx) + ((WFT)dy*(WFT)dy)) + ((WFT)dz*(WFT)dz);
  matrix[19] = (WFT) 1.0; 

  matrix[20] = (WFT) ex;
  matrix[21] = (WFT) ey;
  matrix[22] = (WFT) ez;
  matrix[23] = (((WFT)ex*(WFT)ex) + ((WFT)ey*(WFT)ey)) + ((WFT)ez*(WFT)ez);
  matrix[24] = (WFT) 1.0; 

  WFT det = 0.0; 

  switch (sizeof(WFT) * 8) {
  case 32:
    det = matrix_determinant_32(5, matrix); 
    break;
  case 64:
    det = matrix_determinant_64(5, matrix); 
    break;
  case 128:
    det = matrix_determinant_128(5, matrix); 
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
