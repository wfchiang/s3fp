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
  char *inname = argv[1];
  char *outname = argv[2];

  unsigned int curra = 0;
  unsigned int copya = 1;
  unsigned alen = N;
  unsigned index[2];
  unsigned int i;
  IFT idata;
  OFT odata; 

  FILE *infile = fopen(inname, "r"); 
  FILE *outfile = fopen(outname, "w");
  assert(infile != NULL && outfile != NULL); 
  fseek(infile, 0, SEEK_END); 
  unsigned long fsize = ftell(infile); 
  assert(fsize % sizeof(IFT) == 0);
  N = fsize / sizeof(IFT); 
  fseek(infile, 0, SEEK_SET); 
  assert(N >= 2);

  FT *arr = (FT*) malloc(sizeof(FT) * N); 

  for (i = 0 ; i < N ; i++) {
    fread(&idata, sizeof(IFT), 1, infile);
    arr[i] = (FT) idata;
  }
  fclose(infile);

  while (N > 1) {
    for (i = 0 ; i < N / 2 ; i++) {
      arr[i] += arr[i + (N/2 + (N % 2))]; 
    }
    N = (N / 2) + (N % 2); 
  }

  odata = (OFT) arr[0];
  fwrite(&odata, sizeof(OFT), 1, outfile);
  fclose(outfile);

  return 0;
}

  
