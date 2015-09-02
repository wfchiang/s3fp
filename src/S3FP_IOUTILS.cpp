#include <stdio.h>
#include <assert.h>
extern "C" {
#include <quadmath.h>
}

void InputFP128toFP32 (FILE *infile, float *idata) {
  assert(infile != NULL);
  __float128 fbuf; 
  fread(&fbuf, sizeof(__float128), 1, infile);
  float data32 = (float) fbuf;
  // assert(fbuf == data32);
  (*idata) = data32;
}

void InputFP128toFP64 (FILE *infile, double *idata) {
  assert(infile != NULL);
  __float128 fbuf; 
  fread(&fbuf, sizeof(__float128), 1, infile);
  double data64 = (double) fbuf;
  // assert(fbuf == data64);
  (*idata) = data64;
}

void OutputFP32toFP128 (FILE *outfile, float odata) {
  assert(outfile != NULL);
  __float128 fbuf = odata;
  fwrite(&fbuf, sizeof(__float128), 1, outfile);
}

void OutputFP64toFP128 (FILE *outfile, double odata) {
  assert(outfile != NULL);
  __float128 fbuf = odata;
  fwrite(&fbuf, sizeof(__float128), 1, outfile);
}

