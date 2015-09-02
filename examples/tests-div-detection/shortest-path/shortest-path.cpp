#include <stdio.h>
#include <assert.h>
#include <utility>
#include "s3fp_utils.h"

using namespace std; 

#ifndef IFT 
#define IFT float 
#endif 

#ifndef OFT
#define OFT __float128 
#endif 

unsigned int D = 0; // the number of nodes 

// first : true => is +/- inf 
// first : false => normal length 
typedef pair<bool, WFT> EDIST; 

inline 
EDIST 
edistAdd (EDIST ed1, EDIST ed2) {
  EDIST ret (true, 0);

  if (ed1.first || ed2.first) return ret; 
  else {
    ret.first = false; 
    ret.second = ed1.second + ed2.second; 
  }
  return ret; 
}

inline 
bool 
edistLessThan (EDIST ed1, EDIST ed2) {
  if (ed1.first) return false; 
  if (ed2.first) return true; 
  return ed1.second < ed2.second; 
}

inline 
void SetDist(EDIST *dist, unsigned int s, unsigned int d, EDIST dis) {
  assert(s <= D);
  assert(d <= D); 

  dist[s * D + d] = dis; 
}

inline 
EDIST 
GetDist(EDIST *dist, unsigned int s, unsigned int d) {
  assert(s <= D);
  assert(d <= D); 

  return dist[s * D + d]; 
}

inline 
void DumpDists(EDIST *dist) {
  printf("==== dump dists ====\n"); 
  for (unsigned int s = 0 ; s < D ; s++) {
    for (unsigned int d = 0 ; d < D ; d++) {
      EDIST ed = GetDist(dist, s, d); 
      if (ed.first) 
	printf("+/-inf "); 
      else 
	printf("%5.4f ", (double) ed.second); 
    }
    printf("\n");
  }
  printf("====================\n"); 
}


int main (int argc, char *argv[]) {

  FILE *infile = s3fpGetInFile(argc, argv); 
  FILE *outfile = s3fpGetOutFile(argc, argv); 

  unsigned long fsize = s3fpFileSize(infile); 
  assert(fsize % sizeof(IFT) == 0); 
  fsize = fsize / sizeof(IFT); 
  
  while (true) {
    if (D*D-D == fsize) break; 
    assert((D*D-D) < fsize); 
    D++; 
  }
  assert(D >= 3); 

  EDIST *dist = (EDIST*) malloc(sizeof(EDIST) * D * D); 
  EDIST dzero (false, 0); 
  EDIST dinf (true, 0); 
  
  IFT idata; 
  for (unsigned int s = 0 ; s < D ; s++) {
    for (unsigned int d = 0 ; d < D ; d++) {
      if (s == d) idata = 0; 
      else fread(&idata, sizeof(IFT), 1, infile); 
      SetDist(dist, s, d, EDIST(false, idata));
    }
  }
  /*
#ifdef __VERBOSE
  DumpDists(dist); 
#endif 
  */
  for (unsigned int m = 0 ; m < D ; m++) {
    for (unsigned int s = 0 ; s < D ; s++) {
      for (unsigned int d = 0 ; d < D ; d++) {
        EDIST stom = GetDist(dist, s, m); 
	EDIST mtod = GetDist(dist, m, d); 
	EDIST stod = GetDist(dist, s, d);
	EDIST stom_mtod = edistAdd(stom, mtod); 

	if ( edistLessThan(stom_mtod, stod)) 
	  SetDist(dist, s, d, stom_mtod); 	
      }
    }
  }
  /*
#ifdef __VERBOSE
  DumpDists(dist); 
#endif 
  */
  OFT *odata = (OFT*) malloc(sizeof(OFT) * (D-2)); 

  bool neg_loop = false; 
  for (unsigned int i = 0 ; i < (D-2) ; i++) {
    EDIST ed = GetDist(dist, i, i);
    odata[i] = ed.second; 

    if (ed.first == false) {
      if (ed.second < 0) {
#ifdef __VERBOSE
	printf("NEG LOOP: dist[%d][%d] = %21.20f \n", i, i, (double)ed.second);
#endif 
	neg_loop = true; 
      }
    }
  }

  fwrite(odata, sizeof(OFT), (D-2), outfile); 

  OFT out_det = (neg_loop ? 1.0 : 0.0); 
  fwrite(&out_det, sizeof(OFT), 1,outfile); 

  fclose(infile); 
  fclose(outfile); 

  return 0; 
}
