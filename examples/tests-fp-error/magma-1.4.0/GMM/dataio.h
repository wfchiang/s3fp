#include <quadmath.h>


void fillInputs32 (FILE *infile, float *arr, unsigned int dsize) {
  unsigned int i = 0;
  __float128 indata;
  for (i = 0 ; i < dsize ; i++) {
    fread(&indata, sizeof(__float128), 1, infile);
    arr[i] = (float) indata;
    assert(arr[i] == indata);
  }
}


void fillInputs64 (FILE *infile, double *arr, unsigned int dsize) {
  unsigned int i = 0;
  __float128 indata;
  for (i = 0 ; i < dsize ; i++) {
    fread(&indata, sizeof(__float128), 1, infile);
    arr[i] = (double) indata;
    assert(arr[i] == indata);
  }
}


void writeOutput32 (FILE *outfile, float data32) {
  __float128 outdata = data32;
  fwrite(&outdata, sizeof(__float128), 1, outfile);
}


void writeOutput64 (FILE *outfile, double data32) {
  __float128 outdata = data32;
  fwrite(&outdata, sizeof(__float128), 1, outfile);
}

