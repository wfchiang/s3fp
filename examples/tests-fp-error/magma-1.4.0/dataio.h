
#include <stdio.h> 
#include <quadmath.h>


void fillInputs32to32 (FILE *infile, float *arr, unsigned int dsize) {
  unsigned int i = 0;
  float indata;
  for (i = 0 ; i < dsize ; i++) {
    fread(&indata, sizeof(float), 1, infile);
    arr[i] = (float) indata;
  }
}


void fillInputs32to64 (FILE *infile, double *arr, unsigned int dsize) {
  unsigned int i = 0;
  float indata; 
  for (i = 0 ; i < dsize ; i++) {
    fread(&indata, sizeof(float), 1, infile);
    arr[i] = (double) indata;
  }
}


void writeOutput32to128 (FILE *outfile, float data32) {
  __float128 outdata = data32;
  fwrite(&outdata, sizeof(__float128), 1, outfile);
}


void writeOutput64to128 (FILE *outfile, double data64) {
  __float128 outdata = data64;
  fwrite(&outdata, sizeof(__float128), 1, outfile);
}

