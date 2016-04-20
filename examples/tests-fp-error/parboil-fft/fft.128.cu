/***************************************************************************
 *cr
 *cr            (C) Copyright 2010 The Board of Trustees of the
 *cr                        University of Illinois
 *cr                         All Rights Reserved
 *cr
 ***************************************************************************/

#include <stdio.h>
#include <cuda.h>
// #include <parboil.h>
#include <assert.h>
#include "gqd.cu"
#include "gqd_type.h"
#include <qd/qd_real.h>

#include <iostream>
using namespace std;


#ifndef NN
#define NN 4
#endif 
#ifndef BB
#define BB 4 
#endif 

#ifndef GPUID
#define GPUID 0
#endif 

#define CUERR { cudaError_t err; \
  if ((err = cudaGetLastError()) != cudaSuccess) { \
  printf("CUDA error: %s, line %d\n", cudaGetErrorString(err), __LINE__); \
  return -1; }}

// Block index
#define  bx  blockIdx.x
#define  by  blockIdx.y
// Thread index
#define tx  threadIdx.x

// Possible values are 2, 4, 8 and 16
#ifndef R 
#define R 2
#endif 

#ifndef IFT
#define IFT float 
#endif 

typedef struct {
  gdd_real x;
  gdd_real y;
} gdd_real2; 

inline gdd_real2 __device__ make_gdd_real2 (gdd_real inx, gdd_real iny) {
  gdd_real2 ret;
  ret.x = inx; ret.y = iny;
  return ret;
}

inline gdd_real2 __device__ operator*( gdd_real2 a, gdd_real2 b ) { return make_gdd_real2( a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x ); }
inline gdd_real2 __device__ operator+( gdd_real2 a, gdd_real2 b ) { return make_gdd_real2( a.x + b.x, a.y + b.y ); }
inline gdd_real2 __device__ operator-( gdd_real2 a, gdd_real2 b ) { return make_gdd_real2( a.x - b.x, a.y - b.y ); }
inline gdd_real2 __device__ operator*( gdd_real2 a, gdd_real b ) { return make_gdd_real2( b*a.x , b*a.y); }

#define COS_PI_8  0.923879533f
#define SIN_PI_8  0.382683432f
#define exp_1_16  make_gdd_real2(  make_dd((double)COS_PI_8), make_dd((double)-SIN_PI_8) )
#define exp_3_16  make_gdd_real2(  make_dd((double)SIN_PI_8), make_dd((double)-COS_PI_8) )
#define exp_5_16  make_gdd_real2( make_dd((double)-SIN_PI_8), make_dd((double)-COS_PI_8) )
#define exp_7_16  make_gdd_real2( make_dd((double)-COS_PI_8), make_dd((double)-SIN_PI_8) )
#define exp_9_16  make_gdd_real2( make_dd((double)-COS_PI_8),  make_dd((double)SIN_PI_8) )
#define exp_1_8   make_gdd_real2(  make_dd(1.0), make_dd(-1.0) )
#define exp_1_4   make_gdd_real2(  make_dd(0.0), make_dd(-1.0) )
#define exp_3_8   make_gdd_real2( make_dd(-1.0), make_dd(-1.0) )

dd_real gdd_to_dd (gdd_real fd) {
  dd_real ret;
  ret.x[0] = fd.x;
  ret.x[1] = fd.y;
  return ret;
}

void inputData(FILE* infile, gdd_real2* dat, int num_gdd_real2)
{
  for (unsigned int i = 0 ; i < num_gdd_real2 ; i++) {
    IFT in_data_x, in_data_y; 
    fread(&in_data_x, sizeof(IFT), 1, infile); 
    fread(&in_data_y, sizeof(IFT), 1, infile); 
    dat[i].x = make_dd((double)in_data_x); 
    dat[i].y = make_dd((double)in_data_y); 
  }
}

void outputData(FILE* outfile, gdd_real outdat)
{
  cout << "out_data : " << gdd_to_dd(outdat) << endl;
  fwrite(&(outdat.x), sizeof(double), 1, outfile);
  fwrite(&(outdat.y), sizeof(double), 1, outfile);
}
  
__device__ void GPU_FFT2( gdd_real2 &v1,gdd_real2 &v2 ) { 
  gdd_real2 v0 = v1;  
  v1 = v0 + v2; 
  v2 = v0 - v2; 
}

__device__ void GPU_FFT4( gdd_real2 &v0,gdd_real2 &v1,gdd_real2 &v2,gdd_real2 &v3) { 
   GPU_FFT2(v0, v2);
   GPU_FFT2(v1, v3);
   v3 = v3 * exp_1_4;
   GPU_FFT2(v0, v1);
   GPU_FFT2(v2, v3);    
}


inline __device__ void GPU_FFT2(gdd_real2* v){
  GPU_FFT2(v[0],v[1]);
}

inline __device__ void GPU_FFT4(gdd_real2* v){
  GPU_FFT4(v[0],v[1],v[2],v[3] );
}


inline __device__ void GPU_FFT8(gdd_real2* v){
  GPU_FFT2(v[0],v[4]);
  GPU_FFT2(v[1],v[5]);
  GPU_FFT2(v[2],v[6]);
  GPU_FFT2(v[3],v[7]);

  v[5]=(v[5]*exp_1_8)*make_dd(M_SQRT1_2);
  v[6]=v[6]*exp_1_4;
  v[7]=(v[7]*exp_3_8)*make_dd(M_SQRT1_2);

  GPU_FFT4(v[0],v[1],v[2],v[3]);
  GPU_FFT4(v[4],v[5],v[6],v[7]);
  
}

inline __device__ void GPU_FFT16( gdd_real2 *v )
{
    GPU_FFT4( v[0], v[4], v[8], v[12] );
    GPU_FFT4( v[1], v[5], v[9], v[13] );
    GPU_FFT4( v[2], v[6], v[10], v[14] );
    GPU_FFT4( v[3], v[7], v[11], v[15] );

    v[5]  = (v[5]  * exp_1_8 ) * make_dd(M_SQRT1_2);
    v[6]  =  v[6]  * exp_1_4;
    v[7]  = (v[7]  * exp_3_8 ) * make_dd(M_SQRT1_2);
    v[9]  =  v[9]  * exp_1_16;
    v[10] = (v[10] * exp_1_8 ) * make_dd(M_SQRT1_2);
    v[11] =  v[11] * exp_3_16;
    v[13] =  v[13] * exp_3_16;
    v[14] = (v[14] * exp_3_8 ) * make_dd(M_SQRT1_2);
    v[15] =  v[15] * exp_9_16;

    GPU_FFT4( v[0],  v[1],  v[2],  v[3] );
    GPU_FFT4( v[4],  v[5],  v[6],  v[7] );
    GPU_FFT4( v[8],  v[9],  v[10], v[11] );
    GPU_FFT4( v[12], v[13], v[14], v[15] );
}
     
__device__ int GPU_expand(int idxL, int N1, int N2 ){ 
  return (idxL/N1)*N1*N2 + (idxL%N1); 
}      

__device__ void GPU_FftIteration(int j, int Ns, gdd_real2* data0, gdd_real2* data1){ 
  gdd_real2 v[R];  	
  int idxS = j;       
  gdd_real angle = make_dd((-2*M_PI*(j%Ns)/(Ns*R)));

  for( int r=0; r<R; r++ ) { 
    v[r] = data0[idxS+r*NN/R]; 
    double r64 = (double)r;
    double angle64 = to_double(angle);
    double cos64 = cos(r64 * angle64); // call cuda "cos" instead of the function provided by QD_REAL library 
    double sin64 = sin(r64 * angle64); // call cuda "sin" instead of the function provided by QD_REAL library 
    gdd_real cos_gdd = make_dd(cos64);
    gdd_real sin_gdd = make_dd(sin64);

    v[r] = v[r] * make_gdd_real2(cos_gdd, sin_gdd);
  }       

#if R == 2 
  GPU_FFT2( v ); 
#endif

#if R == 4
  GPU_FFT4( v );
#endif	 	

#if R == 8
  GPU_FFT8( v );
#endif

#if R == 16
  GPU_FFT16( v );
#endif	 	

  int idxD = GPU_expand(j,Ns,R); 

  for( int r=0; r<R; r++ ){
    data1[idxD+r*Ns] = v[r];	
  } 	

}      

__global__ void GPU_FFT_Global(int Ns, gdd_real2* data0, gdd_real2* data1) { 
  data0+=bx*NN;
  data1+=bx*NN;	 
  GPU_FftIteration( tx, Ns, data0, data1);  
}      

int main( int argc, char **argv )
{	
  int n_bytes; 

  // check config 
  assert(NN > 0);
  assert(BB > 0);
  assert(R > 0);
  unsigned int nn = NN;
  unsigned int rr = R; 
  while (nn > 0) {
    if (nn == 1) break;
    assert(nn / rr > 0); 
    nn = nn / rr; 
  }    
  
  // get IO file name 
  assert(argc == 3);
  char *inname = argv[1];
  char *outname = argv[2];
  
  // int N, B;
  n_bytes = NN*BB*sizeof(gdd_real2);

  cudaSetDevice(GPUID);

  gdd_real2 *source;
  gdd_real2 *result;
  cudaMallocHost((void**)&source, n_bytes);
  CUERR;
  cudaMallocHost((void**)&result, n_bytes);
  CUERR;

  // allocate device memory
  gdd_real2 *d_source, *d_work;
  cudaMalloc((void**) &d_source, n_bytes);
  CUERR;
  cudaMalloc((void**) &d_work, n_bytes);
  CUERR;

  FILE * infile = fopen(inname, "r");
  assert(infile != NULL);
  fseek(infile, 0, SEEK_END);
  long fsize = ftell(infile);
  fseek(infile, 0, SEEK_SET);
  assert(fsize % (NN*BB*sizeof(IFT)*2) == 0);
  unsigned int n_repeats = fsize / (NN*BB*sizeof(IFT)*2);

  FILE *outfile = fopen(outname, "w");
  assert(outfile != NULL);

  for (unsigned int ri = 0 ; ri < n_repeats ; ri++) {

    inputData(infile, (gdd_real2*)source,NN*BB);
  
    // copy host memory to device
    cudaMemcpy(d_source, source, n_bytes,cudaMemcpyHostToDevice);
    CUERR;
    cudaMemset(d_work, 0,n_bytes);
    CUERR;

    for( int Ns=1; Ns<NN; Ns*=R){
      GPU_FFT_Global<<<dim3(BB), dim3(NN/R)>>>(Ns, d_source, d_work);
      gdd_real2 *tmp = d_source;
      d_source = d_work;
      d_work = tmp;
    }

    // copy device memory to host
    cudaMemcpy(result, d_source, n_bytes,cudaMemcpyDeviceToHost);
    CUERR;

    outputData(outfile, (gdd_real)result[NN*BB-1].y);

  }

  cudaFree(d_source);
  CUERR;
  cudaFree(d_work);
  CUERR;	

  cudaFreeHost(source);
  cudaFreeHost(result);

  fclose(infile);
  fclose(outfile);

  return 0;
}

