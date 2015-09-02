#include <stdio.h>
#include <assert.h>
#include "quadmath.h"

#ifndef IFT
#define IFT float 
#endif 

#ifndef OFT 
#define OFT __float128 
#endif 

#ifndef WFT 
#define WFT float
#endif

unsigned int N = 0; 


int main (int argc, char *argv[]) {
  assert(sizeof(WFT) == sizeof(float) ||
	 sizeof(WFT) == sizeof(double) || 
	 sizeof(WFT) == sizeof(__float128));

  unsigned int i = 0; 
  char *inname = argv[1];
  char *outname = argv[2];
  
  FILE *infile = fopen(inname, "r"); 
  fseek(infile, 0, SEEK_END); 
  unsigned long fsize = ftell(infile); 
  assert(fsize % sizeof(IFT) == 0); 
  N = fsize / sizeof(IFT); 
  fseek(infile, 0, SEEK_SET); 
  
  assert(N > 1);

  WFT *arr = (WFT*) malloc(sizeof(WFT) * N); 

  for (i = 0 ; i < N ; i++) {
    IFT in_data;
    fread(&in_data, sizeof(IFT), 1, infile);
    arr[i] = (WFT) in_data;
  }
  fclose(infile);

  WFT compensate = 0.0; 
  for (i = 1 ; i < N ; i++) {
    WFT interm = arr[i] - compensate;
    WFT temp = arr[0] + interm; 
    compensate = (temp - arr[0]) - interm;
    arr[0] = temp; 
  }

  FILE *outfile = fopen(outname, "w");
  OFT out_data = (OFT) arr[0];
  fwrite(&out_data, sizeof(OFT), 1, outfile);
  fclose(outfile);

  return 0;
}
