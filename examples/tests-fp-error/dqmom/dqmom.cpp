/*
 ============================================================================
 Name        : Arches.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <quadmath.h>
#include <string>

using namespace std; 


#ifndef FT
#define FT float
#endif 

#ifndef IFT 
#define IFT float 
#endif 


#ifdef EXP1 // experiment 1 setup, totally 9 input vars (3 in models array)
#define num_iter 1
#define num_cells 1
#define indicesSize 12
#define N_ 3
#define N_xi 1
#define blocks 1
#define threads 36 // NOTE: the # of threads should be 3x of indicesSize
#endif 

#ifdef EXP2 // experiment 2 setup, totally 15 input vars (6 in models array) 
#define num_iter 1
#define num_cells 1
#define indicesSize 12
#define N_ 3
#define N_xi 2
#define blocks 1
#define threads 36 // NOTE: the # of threads should be 3x of indicesSize
#endif 

#ifdef EXP3 // experiment setting 3, totally 30 input vars (12 in models array)
#define num_iter 1
#define num_cells 2
#define indicesSize 24
#define N_ 3
#define N_xi 2
#define blocks 2
#define threads 72 // NOTE: the # of threads should be 3x of indicesSize
#endif 

#ifdef EXP4 // experiment setting 4 (totally 960 inputs, 385 in models) 
#define num_iter 1
#define num_cells 64
#define indicesSize 24
#define N_ 3
#define N_xi 2
#define blocks 4
#define threads 72 // NOTE: the # of threads should be 3x of indicesSize
#endif 

#ifdef EXP5 // experiment setting 5 (totally 240 inputs, 96 in models) 
#define num_iter 1
#define num_cells 16
#define indicesSize 24
#define N_ 3
#define N_xi 2
#define blocks 4
#define threads 72 // NOTE: the # of threads should be 3x of indicesSize
#endif 


FT my_pow (FT base, FT expo) {
  int i;
  FT ret = (FT) 1;
  FT l_base = (FT) 1;

  if (base == 0) {
    assert(expo != 0 && "ERROR [my_pow]: Can not calculate 0^0");
    return (FT) 0; 
  }
  else {
    if (expo == 0) return ret; 
    else if (expo > 0) {
      for (i = 0 ; i < expo ; i++) {
	l_base = l_base * base;
      }
      ret = l_base;
    }
    else { // expo < 0
      for (i = 0 ; i < expo ; i++) {
	l_base = l_base / base;
      }
      ret = l_base;
    }
    return ret; 
  }
}


int main(int argc, char *argv[]) {
  const string inputname = argv[1];
  const string outputname = argv[2];

  FILE *infile = fopen(inputname.c_str(), "r");
  assert(infile != NULL);

  //initialize the data
  int momentIndicesArray[indicesSize*N_xi];
  unsigned int k,m;
  for(k = 0; k<indicesSize; k++) {
    int *mv = &(momentIndicesArray[k*N_xi]);
    for(m = 0; m<N_xi; m++) {
      // mv[m] = rand()%4;
      mv[m] = 3;
    }
  }
  
  FT *weightsArray = (FT *)malloc(sizeof(FT)*num_cells*N_);
  for (unsigned int vi = 0 ; vi < num_cells*N_ ; vi++) {
    IFT data; 
    fread(&data, sizeof(IFT), 1, infile);
    weightsArray[vi] = (FT) data;
  }

  FT *weightedAbscissasArray = (FT *)malloc(sizeof(FT)*num_cells*N_xi*N_);
  for (unsigned int vi = 0 ; vi < num_cells*N_xi*N_ ; vi++) {
    IFT data;
    fread(&data, sizeof(IFT), 1, infile);
    weightedAbscissasArray[vi] = (FT) data;
  }

  FT *modelsArray = (FT *)malloc(sizeof(FT)*num_cells*N_xi*N_);
  for (unsigned int vi = 0 ; vi < num_cells*N_xi*N_ ; vi++) {
    IFT data;
    fread(&data, sizeof(IFT), 1, infile);
    modelsArray[vi] = (FT) data;
  }

  fclose(infile);
  
  //Now try the two different solve methods
  FT *AAArray_cpu = (FT *)malloc(num_cells*indicesSize*N_xi*N_*sizeof(FT));
  FT *BBArray_cpu = (FT *)malloc(num_cells*indicesSize*sizeof(FT));

  int cell_num;
  unsigned int j,n,alpha,i,iter;

  for(iter=0; iter<num_iter; iter++) { 
    // construct AX=B
    for(cell_num=0; cell_num<num_cells; cell_num++) {
      FT *weights = &(weightsArray[cell_num*N_]);
      FT *weightedAbscissas = &(weightedAbscissasArray[cell_num*N_*N_xi]);
      FT *models = &(modelsArray[cell_num*N_*N_xi]);
      FT *AA = &(AAArray_cpu[cell_num*indicesSize*N_*N_xi]);
      FT *BB = &(BBArray_cpu[cell_num*indicesSize]);
      
      // construct AX=B
      for ( k = 0; k < indicesSize; ++k) {
	int *thisMoment = &(momentIndicesArray[k*N_xi]);

	//preprocessing
	//start with powers
	FT d_powers[N_][N_xi]; //a^(b-1)
	FT powers[N_][N_xi]; //a^b
	FT rightPartialProduct[N_][N_xi], leftPartialProduct[N_][N_xi];
	for ( unsigned int m = 0; m < N_; m++) {
	  if(weights[m]!=0) {
	    for ( unsigned int n = 0; n < N_xi; n++) {
	      //TODO should we worry about 0^0, based on former seq code, only fear is resolved
	      FT base = weightedAbscissas[n*N_+m]/weights[m];
	      FT exponent = (FT) (thisMoment[n]-1);
	      d_powers[m][n] = my_pow(base,exponent);
	      powers[m][n] = d_powers[m][n]*base;
	    }
	  }
	  else {
	    for ( unsigned int n = 0; n < N_xi; n++) {
	      d_powers[m][n] = powers[m][n] = 0;
	      rightPartialProduct[m][n] = leftPartialProduct[m][n] = 0;
	    }
	    rightPartialProduct[m][0] = leftPartialProduct[m][N_xi-1] = 1;
	  }
	}
	//now partial products to eliminate innermost for loop
	for(unsigned int m = 0; m < N_; m++) {
	  if(weights[m] != 0) {
	    rightPartialProduct[m][0] = 1;
	    leftPartialProduct[m][N_xi-1] = 1;
	    for( unsigned int n = 1; n< N_xi; n++) {
	      rightPartialProduct[m][n] = rightPartialProduct[m][n-1]*powers[m][n-1];
	      leftPartialProduct[m][N_xi-1-n] = leftPartialProduct[m][N_xi-1-n+1]*powers[m][N_xi-1-n+1];
	    }
	  }
	}
	//end preprocessing
	
	// weights
	for ( alpha = 0; alpha < N_; ++alpha) {
	  FT prefixA = 1;
	  FT productA = 1;
	  for ( i = 0; i < N_xi; ++i) {
	    prefixA = prefixA - (thisMoment[i]);
	    productA = productA*( powers[alpha][i] );
	  }
	  AA[k*N_*N_xi+alpha] = prefixA*productA;
	} //end weights sub-matrix
	
	// weighted abscissas
	FT totalsumS = 0;
	for( j = 0; j < N_xi; ++j ) {
	  FT prefixA    = 1;
	  FT productA   = 1;
	  
	  FT prefixS    = 1;
	  FT productS   = 1;
	  FT modelsumS  = 0;
	  
	  FT quadsumS = 0;
	  for( alpha = 0; alpha < N_; ++alpha ) {
	    if (weights[alpha] == 0) {
	      prefixA = 0;
	      prefixS = 0;
	      productA = 0;
	      productS = 0;
	    } else if ( weightedAbscissas[j*(N_)+alpha] == 0 && thisMoment[j] == 0) {
	      //FIXME:
	      // both prefixes contain 0^(-1)
	      prefixA = 0;
	      prefixS = 0;
	    } else {
	      prefixA = (thisMoment[j])*( d_powers[alpha][j] ); 
	      productA = 1;

	      prefixS = -(thisMoment[j])*( d_powers[alpha][j] ); 
	      productS = 1;
	      
	      productA = productS = rightPartialProduct[alpha][j]*leftPartialProduct[alpha][j];
	    }//end divide by zero conditionals
	    
	    
	    modelsumS = - models[j*(N_)+alpha];
	    
	    int col = (j+1)*N_ + alpha;
	    if(j+1<N_xi) {
	      AA[k*N_*N_xi+col] = prefixA*productA;
	    }
	    
	    quadsumS = quadsumS + weights[alpha]*modelsumS*prefixS*productS;
	  }//end quad nodes
	  totalsumS = totalsumS + quadsumS;
	}//end int coords j sub-matrix
	BB[k] = totalsumS;
      } // end moments
    }// end cellsA
  }

  FILE *outfile = fopen(outputname.c_str(), "w");
  assert(outfile != NULL);

#ifdef CHECK_AA
  #ifdef CHECK_BB
    printf("ERROR: Can't check both AA and BB\n");
  #else 
    __float128 out_data = (__float128) AAArray_cpu[(num_cells*indicesSize*N_xi*N_)/2];
    fwrite(&out_data, sizeof(__float128), 1, outfile);
  #endif // CHECK_BB
#else
  #ifdef CHECK_BB
    __float128 out_data = (__float128) BBArray_cpu[(num_cells*indicesSize)/2];
    printf("%f\n", (double)out_data); 
    fwrite(&out_data, sizeof(__float128), 1, outfile);
  #else
    printf("ERROR: Need to either define CHECK_AA or CHECK_BB\n");
  #endif 
#endif // CHECK_AA 

  fclose(outfile);
    
  return 0;
}
