#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include "div_geometric_primitives.h"

using namespace std; 


#ifndef FT 
#define FT float 
#endif 

#ifndef RSEED
#define RSEED 0
#endif 


int main (void) {
  
  FT *m3x3 = (FT*) malloc(sizeof(FT) * 3 * 3);
  FT *m4x4 = (FT*) malloc(sizeof(FT) * 4 * 4);
  FT *m5x5 = (FT*) malloc(sizeof(FT) * 5 * 5);

  srand(RSEED); 

  for (int ix = 0 ; ix < 3 ; ix++) {
    for (int iy = 0 ; iy < 3 ; iy++) {
      m3x3[ix*3 + iy] = (FT)(rand() % 1000000) / (FT)100000;
      cout << (long double) m3x3[ix*3 + iy] << " "; 
    }
    cout << endl;
  }

  for (int ix = 0 ; ix < 4 ; ix++) {
    for (int iy = 0 ; iy < 4 ; iy++) {
      m4x4[ix*4 + iy] = (FT)(rand() % 1000000) / (FT)100000;
      cout << (long double) m4x4[ix*4 + iy] << " "; 
    }
    cout << endl;
  }

  for (int ix = 0 ; ix < 5 ; ix++) {
    for (int iy = 0 ; iy < 5 ; iy++) {
      m5x5[ix*5 + iy] = (FT)(rand() % 1000000) / (FT)100000;
      cout << (long double) m5x5[ix*5 + iy] << " ";  
    }
    cout << endl;
  }

  m5x5[0] = 1.0;  
  m5x5[1] = 1.0;  
  m5x5[2] = 0.0;  
  m5x5[3] = 2.0;
  m5x5[4] = 1.0;  

  m5x5[5] = 1.0;  
  m5x5[6] = -1.0;  
  m5x5[7] = 0.0;  
  m5x5[8] = 2.0;
  m5x5[9] = 1.0;  

  m5x5[10] = -1.0;  
  m5x5[11] = 1.0;  
  m5x5[12] = 0.0;  
  m5x5[13] = 2.0;
  m5x5[14] = 1.0;  

  m5x5[15] = -1.0;  
  m5x5[16] = -1.0;  
  m5x5[17] = 0.0;  
  m5x5[18] = 2.0;
  m5x5[19] = 1.0;  

  m5x5[20] = 100.0;  
  m5x5[21] = 100.0;  
  m5x5[22] = 100.0;  
  m5x5[23] = 30000.0;
  m5x5[24] = 1.0;  

  FT det3x3;
  FT det4x4;
  FT det5x5; 

  switch (sizeof(FT) * 8) {
  case 32:   
    det3x3 = matrix_determinant_32(3, m3x3); 
    det4x4 = matrix_determinant_32(4, m4x4); 
    det5x5 = matrix_determinant_32(5, m5x5); 
    break;
  case 64:
    det3x3 = matrix_determinant_64(3, m3x3); 
    det4x4 = matrix_determinant_64(4, m4x4); 
    det5x5 = matrix_determinant_64(5, m5x5); 
    break;
  case 128:
    det3x3 = matrix_determinant_128(3, m3x3); 
    det4x4 = matrix_determinant_128(4, m4x4); 
    det5x5 = matrix_determinant_128(5, m5x5); 
    break;
  default:
    assert(false); 
    break;
  }				  

  cout << "det3x3: " << (long double) det3x3 << endl;
  cout << "det4x4: " << (long double) det4x4 << endl;
  cout << "det5x5: " << (long double) det5x5 << endl;
  
  return 0; 
}
