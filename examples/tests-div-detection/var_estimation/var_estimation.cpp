#include <stdio.h>
#include <math.h>
#include <assert.h>
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

unsigned int N = 0; 


WFT VarEstimation (IFT *inxs) {
  WFT ret = 0.0; 
  WFT sum = 0.0, sum2 = 0.0; 
  WFT *xs = (WFT*) malloc(sizeof(WFT) * N); 

  for (int i = 0 ; i < N ; i++) 
    xs[i] = (WFT) inxs[i]; 

  for (int i = 0 ; i < N ; i++) {
    sum += xs[i]; 
    sum2 += (xs[i] * xs[i]); 
  }
  
  WFT ave2 = sum2 / (WFT) N; 
  WFT ave = sum / (WFT) N; 

  ret = ave2 - (ave * ave); 
  
  /*
  ret = sum2 - ((sum * sum) / (WFT)N); 
  ret = ret / (WFT)(N-1); 
  */
    
  return ret; 
}


int main (int argc, char *argv[]) {
  assert(argc == 3); 
  assert(sizeof(WFT) >= sizeof(IFT));
 
  FILE *infile = s3fpGetInFile(argc, argv); 
  FILE *outfile = s3fpGetOutFile(argc, argv); 
  assert(infile != NULL && outfile != NULL); 

  unsigned long fsize = s3fpFileSize(infile); 
  assert(fsize % sizeof(IFT) == 0); 
  N = fsize / sizeof(IFT); 

  IFT *inxs = (IFT*) malloc(sizeof(IFT) * N); 
  s3fpGetInputs<IFT>(infile, N, inxs); 

  WFT rel = VarEstimation(inxs); 

  OFT rel_128 = (OFT) rel; 
  OFT sign_128 = (((sizeof(WFT) == sizeof(__float128)) || 
		   (0 <= rel_128)) 
		  ? 1.0 : -1.0);

#ifdef __VERBOSE
  printf("var: %21.20f \n", (double) rel_128); 
#endif 

  fwrite(&rel_128, sizeof(OFT), 1, outfile); 
  fwrite(&sign_128, sizeof(OFT), 1, outfile); 

  fclose(infile); 
  fclose(outfile); 

  return 0; 
}
