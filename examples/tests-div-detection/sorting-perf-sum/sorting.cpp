#include <stdio.h>
#include <math.h>
#include <iostream>
#include <assert.h>
extern "C" {
#include "quadmath.h"
}
#include "s3fp_utils.h"

using namespace std; 

#ifndef IFT 
#define IFT float 
#endif 

#ifndef OFT
#define OFT __float128 
#endif 

unsigned int D = 0; 
unsigned int N = 0; 


void PrepareDefaultInput (char *inname) {
  FILE *infile = fopen(inname, "w"); 
  
  IFT idata; 
  for (int i = 0 ; i < 4 ; i++) {
    for (int j = 0 ; j < 4 ; j++) {
      idata = 4.5 - (1.1 * (i * 2 + j)); \
      fwrite(&idata, sizeof(IFT), 1, infile); 
    }
  }

  fclose(infile); 
}


template<class THISFT> 
THISFT 
ImbalancedReduction (THISFT *fps) {
  THISFT sum = 0.0; 
  for (int i = 0 ; i < N ; i++) {
    sum += fps[i]; 
  }
  return sum; 
}


template<class THISFT> 
THISFT 
BalancedReduction (THISFT *fps) {
  assert(N > 0); 
  int i ; 
  if (N == 1) return fps[0]; 

  THISFT *buff = (THISFT *) malloc(sizeof(THISFT) * N); 
  
  for (i = 0 ; i < N ; i++) 
    buff[i] = fps[i]; 

  unsigned int currN = N; 
  unsigned int halfN = currN / 2; 
  
  while (currN > 1) {
    for (i = 0 ; i < halfN ; i++) 
      buff[i] += buff[i+halfN]; 
    if (halfN * 2 == currN) 
      currN = halfN; 
    else {
      buff[i] = buff[currN-1]; 
      currN = halfN + 1; 
    }
    halfN = currN / 2; 
  }

  return buff[0]; 
}


template<class THISFT> 
void 
BobbleSort (THISFT *sums, unsigned int *indices) { 
  unsigned int temp_id; 

  for (int i = 0 ; i < D ; i++) {
    indices[i] = i; 
  }

  for (int i = D ; i > 1 ; i--) {
    for (int j = 0 ; j < (i-1) ; j++) {
      if ((sums[indices[j]] > sums[indices[j+1]]) || 
	  ((sums[indices[j]] == sums[indices[j+1]]) && 
	   (indices[j] > indices[j+1]))) {
	temp_id = indices[j]; 
	indices[j] = indices[j+1]; 
	indices[j+1] = temp_id; 
      }
    }
  }
}


int main (int argc, char *argv[]) {
  assert(argc == 5); 

  FILE *infile = s3fpGetInFile(argc, argv); 
  FILE *outfile = s3fpGetOutFile(argc, argv); 

  // reduction selection 
  // if rmethod == 0 -->> imbalanced reduction 
  // if rmethod == 1 -->> balanced reduction 
  int rmethod = atoi(argv[1]); 
  assert(rmethod == 0 || rmethod == 1); 

  // if vmethod == 0 -->> single fp value
  // if vmethod == 1 -->> multiple fp values  
  int vmethod = atoi(argv[2]); 
  assert(vmethod == 0 || vmethod == 1); 

  unsigned long fsize = s3fpFileSize(infile); 
  assert(fsize % sizeof(IFT) == 0);
  fsize = fsize / sizeof(IFT); 
  assert(fsize > 1); 

  D = N = 1; 
  while (true) {
    if (fsize == D * N) break; 
    assert(fsize > (D * N)); 
    D++; 
    N++; 
  }
    
  WFT **farrays = (WFT**) malloc(sizeof(WFT*) * D); 
  for (int i = 0 ; i < D ; i++) 
    farrays[i] = (WFT*) malloc(sizeof(WFT) * N); 
    
  IFT idata; 
  for (int i = 0 ; i < D ; i++) {
    for (int j = 0 ; j < N ; j++) {
      fread(&idata, sizeof(IFT), 1, infile); 
      farrays[i][j] = idata; 
    }
  }

  WFT *frels = (WFT*) malloc(sizeof(WFT) * D); 
  for (int i = 0 ; i < D ; i++) {
    switch (rmethod) {
    case 0: 
      frels[i] = ImbalancedReduction<WFT>(farrays[i]); 
      break; 
    case 1: 
      frels[i] = BalancedReduction<WFT>(farrays[i]); 
      break; 
    default: 
      assert(false && "ERROR: No such reduction method..."); 
      break; 
    }

    if (i == 0) 
      printf("frels[%d] = %9.8f \n", i, (double) frels[i]); 
  }

  unsigned int *indices = (unsigned int *) malloc(sizeof(unsigned int) * D); 
  BobbleSort<WFT>(frels, indices); 

  OFT vs_length = D; 
  OFT odata;
  
  fwrite(&vs_length, sizeof(OFT), 1, outfile); 

  for (int i = 0 ; i < D ; i++) {
    if (vmethod == 0) {
      if (i != (D-1))
	odata = 1.0; 
      else 
	odata = (frels[indices[D-1]] - frels[indices[D-2]]); 
    }
    else if (vmethod == 1) {
      if (i == 0) 
	odata = 1.0; 
      else 
	odata = (frels[indices[i]] - frels[indices[i-1]]); 
    }
    else assert(false && "ERROR: no such vmethod"); 
    
    fwrite(&odata, sizeof(OFT), 1, outfile); 
    
#ifdef __VERBOSE
    printf("frels[%d] : %6.5f \n", i, (double) frels[i]); 
#endif 
  }

  vs_length = D + 1; 
  fwrite(&vs_length, sizeof(OFT), 1, outfile); 
  OFT sbound = 0; // sqrt((double)D); 
  fwrite(&sbound, sizeof(OFT), 1, outfile); 

  for (int i = 0 ; i < D ; i++) {
    int j; 
    for (j = 0 ; j < D ; j++) {
      if (indices[j] != i) continue; 
      odata = j; 
      fwrite(&odata, sizeof(OFT), 1, outfile); 

#ifdef __VERBOSE
      printf("%d's order : %d\n", i, j); 
#endif 
      break; 
    }
    assert(j < D); 
  }

  fclose(infile); 
  fclose(outfile); 
  
  return 0; 
}


