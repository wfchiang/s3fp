#include <stdio.h>
#include <assert.h>
extern "C" {
#include <quadmath.h>
}

#ifndef FT
#define FT float
#endif 

#ifndef IFT 
#define IFT float 
#endif 

#ifndef OFT 
#define OFT __float128 
#endif 

unsigned long N = 0; 


int main (int argc, char *argv[]) { 
  assert(sizeof(FT) == sizeof(float) ||
	 sizeof(FT) == sizeof(double) || 
	 sizeof(FT) == sizeof(__float128));
  assert(argc == 3);

  int i;
  char *inname = argv[1];
  char *outname = argv[2];
  FILE *infile = fopen(inname, "r");
  FILE *outfile = fopen(outname, "w");
  fseek(infile, 0, SEEK_END); 
  unsigned long fsize = ftell(infile); 
  assert(fsize % sizeof(IFT) == 0); 
  N = fsize / sizeof(IFT); 
  fseek(infile, 0, SEEK_SET); 

  assert(N >= 2);

  FT *L = (FT*) malloc(sizeof(FT) * N); 

  for (i = 0 ; i < N ; i++) {
    IFT in_data;
    fread(&in_data, sizeof(IFT), 1, infile);
    L[i] = (FT) in_data;
  }
  fclose(infile);

  for (i = N-1 ; i > 0 ; i--) 
    L[i-1] += L[i];

  OFT out_data = (OFT) L[0];
  fwrite(&out_data, sizeof(OFT), 1, outfile);
  fclose(outfile);

  return 0;
}
