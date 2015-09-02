#include "s3fp_utils_gpu.h"
extern "C" {
#include "quadmath.h"
}


FILE * 
s3fpGetInFile (int argc, char **argv) {
  assert(argc == 3); 
  
  char *inname = argv[argc-2]; 
  assert(inname != NULL); 
  
  FILE *infile = fopen(inname, "r"); 
  assert(infile != NULL); 
  
  return infile; 
}


FILE * 
s3fpGetOutFile (int argc, char **argv) {
  assert(argc == 3); 

  char *outname = argv[argc-1]; 
  assert(outname != NULL); 
  
  FILE *outfile = fopen(outname, "w"); 
  assert(outfile != NULL); 

  return outfile; 
}


unsigned long 
s3fpFileSize (FILE *f) {
  assert(f != NULL); 

  unsigned long curr = ftell(f); 

  fseek(f, 0, SEEK_END); 
  unsigned long fsize = ftell(f); 

  fseek(f, curr, SEEK_SET); 
  
  return fsize; 
}


void 
s3fpGetInputs32 (FILE *infile, 
		 unsigned long n_inputs, 
		 float *iarr) {
  unsigned long fsize = s3fpFileSize(infile); 
  
  assert(fsize == n_inputs * sizeof(float)); 
  
  fread(iarr, sizeof(float), n_inputs, infile); 
}


void 
s3fpWriteOutputs32 (FILE *outfile, 
		    unsigned int n_outputs, 
		    float *oarr) {
  assert(outfile != NULL); 

  fwrite(oarr, sizeof(float), n_outputs, outfile); 
}


void 
s3fpWriteOutputs64 (FILE *outfile, 
		    unsigned int n_outputs, 
		    double *oarr) {
  assert(outfile != NULL); 

  fwrite(oarr, sizeof(double), n_outputs, outfile); 
}
