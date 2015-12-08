#include "s3fp_utils.h"

#ifndef IFT 
#define IFT float 
#endif 

#ifndef OFT 
#define OFT __float128 
#endif 


unsigned int N = 0; 


void 
naive_scan (WFT *data) {
  WFT *buff = (WFT*) malloc(sizeof(WFT) * N * 2); 

  buff[0] = 0.0; 
  buff[N] = 0.0; 

  for (unsigned int i = 1 ; i < N ; i++) {
    buff[i] = data[i-1]; 
    buff[N + i] = data[i-1]; 
  }

  unsigned int dest_offset = 0; 
  unsigned int sour_offset = N; 

  for (unsigned int i = 1 ; i < N ; i = i*2) {
    dest_offset = N - dest_offset; 
    sour_offset = N - sour_offset; 

    for (unsigned int ii = 0 ; ii < N ; ii++) {
      if (ii < i) 
	buff[dest_offset + ii] = buff[sour_offset + ii]; 
      else 
	buff[dest_offset + ii] = 
	  buff[sour_offset + ii] + buff[sour_offset + (ii - i)]; 
    }
  }

  for (unsigned int i = 0 ; i < N ; i++) 
    data[i] = buff[dest_offset + i]; 

  free(buff); 
}


void 
harris_scan (WFT *data) {
  WFT *buff = (WFT*) malloc(sizeof(WFT) * N); 

  for (unsigned int i = 0 ; i < N ; i++) 
    buff[i] = data[i]; 

  for (unsigned int i = 2 ; i <= N ; i = i*2) {
    for (unsigned int ii = 0 ; ii < N ; ii++) {
      if ((ii+1) % i == 0) 
	buff[ii] = buff[ii] + buff[ii - (i / 2)]; 
    }
  }

  buff[N-1] = 0; 

  for (unsigned int i = N ; i >= 2 ; i = i/2) {
    for (unsigned int ii = 0 ; ii < N ; ii++) {
      if ((ii+1) % i == 0) {
	WFT temp = buff[ii - (i / 2)]; 
	buff[ii - (i / 2)] = buff[ii]; 
	buff[ii] += temp; 
      }
    }
  }
}


int main (int argc, char *argv[]) {
  assert(argc == 5); 

  FILE *infile = s3fpGetInFile(argc, argv); 
  FILE *outfile = s3fpGetOutFile(argc, argv); 

  int smethod = atoi(argv[1]); 
  int optmethod = atoi(argv[2]); 
  
  unsigned int fsize  = s3fpFileSize(infile); 
  assert(fsize % sizeof(IFT) == 0); 
  N = fsize / sizeof(IFT); 
  assert(N >= 2); 
  
  int b = 1; 
  while (true) {
    if (b == N) break; 
    assert(b < N); 
    b = b * 2; 
  }
  
  IFT *idata = (IFT *) malloc(sizeof(IFT) * N); 
  s3fpGetInputs<IFT>(infile, N, idata); 
  
  WFT *wdata = (WFT*) malloc(sizeof(WFT) * N);
  for (unsigned int i = 0 ; i < N ; i++) 
    wdata[i] = idata[i]; 

  switch (smethod) {
  case 0: 
    naive_scan(wdata); 
    break;
  case 1: 
    harris_scan(wdata); 
    break;
  default: 
    assert(false); 
  }
  
#ifdef __VERBOSE
  for (unsigned int i = 0 ; i < N ; i++) 
    printf("[%d] : %8.7f \n", i, (double) wdata[i]); 
#endif 

  bool invariant_violated = false; 

  OFT *odata = (OFT*) malloc(sizeof(OFT) * N);

  for (unsigned int i = 1 ; i < N ; i++) {
    odata[i-1] = (OFT)(wdata[i] - wdata[i-1]); 
    invariant_violated = (invariant_violated || 
			  (wdata[i] >= wdata[i-1])); 
  }
  
  odata[N-1] = (invariant_violated ? 1.0 : -1.0); 
  
  if (optmethod == 0) {
    OFT od = 1; 
    fwrite(&od, sizeof(OFT), 1, outfile); 
    fwrite(&odata[N-2], sizeof(OFT), 1, outfile); 
    fwrite(&od, sizeof(OFT), 1, outfile); 
    fwrite(&odata[N-1], sizeof(OFT), 1, outfile); 
  }
  else if (optmethod == 1) {
    OFT vs_length = N - 1; 
    assert(vs_length >= 1); 
    fwrite(&vs_length, sizeof(OFT), 1, outfile); 
    s3fpWriteOutputs<OFT>(outfile, N-1, odata); 
    vs_length = 1; 
    fwrite(&vs_length, sizeof(OFT), 1, outfile); 
    fwrite(&odata[N-1], sizeof(OFT), 1, outfile); 
  }
  else assert(false && "ERROR: No such opt method..."); 

  fclose(infile); 
  fclose(outfile); 
  
  return 0; 
}
