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
#include <qd/qd_real.h>

#include <iostream>
using namespace std;


#ifndef FT 
#define FT float 
#define FT2 float2
#define make_FT2 make_float2
#endif

#ifndef IFT
#define IFT float 
#endif  

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

inline FT2 __device__ operator*( FT2 a, FT2 b ) { return make_FT2( a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x ); }
inline FT2 __device__ operator+( FT2 a, FT2 b ) { return make_FT2( a.x + b.x, a.y + b.y ); }
inline FT2 __device__ operator-( FT2 a, FT2 b ) { return make_FT2( a.x - b.x, a.y - b.y ); }
inline FT2 __device__ operator*( FT2 a, FT b ) { return make_FT2( b*a.x , b*a.y); }

#define COS_PI_8  0.923879533f
#define SIN_PI_8  0.382683432f
#define exp_1_16  make_FT2(  COS_PI_8, -SIN_PI_8 )
#define exp_3_16  make_FT2(  SIN_PI_8, -COS_PI_8 )
#define exp_5_16  make_FT2( -SIN_PI_8, -COS_PI_8 )
#define exp_7_16  make_FT2( -COS_PI_8, -SIN_PI_8 )
#define exp_9_16  make_FT2( -COS_PI_8,  SIN_PI_8 )
#define exp_1_8   make_FT2(  1, -1 )
#define exp_1_4   make_FT2(  0, -1 )
#define exp_3_8   make_FT2( -1, -1 )

void inputData(FILE* fid, FT2* dat, int num_ft2)
{
  assert(sizeof(FT) == 4 || sizeof(FT) == 8); 

  if (fid == NULL)
    {
      fprintf(stderr, "Cannot open input file\n");
      exit(-1);
    }
  for (unsigned int i = 0 ; i < num_ft2 ; i++) {
    IFT in_data;
    fread(&(in_data), sizeof(IFT), 1, fid);
    if (sizeof(FT) == 4) 
      dat[i].x = (float) in_data;
    else // (sizeof(FT) == 8) 
      dat[i].x = (double) in_data; 
    fread(&(in_data), sizeof(IFT), 1, fid);
    if (sizeof(FT) == 4) 
      dat[i].y = (float) in_data; 
    else // (sizeof(FT) == 8) 
      dat[i].y = (double) in_data; 
  }
  /*
  for (unsigned int i = 0 ; i < num_ft2 ; i++) {
    dd_real in_data;
    fread(&(in_data.x[0]), sizeof(double), 1, fid);
    fread(&(in_data.x[1]), sizeof(double), 1, fid);
    if (sizeof(FT) == 4) 
      dat[i].x = (float) to_double(in_data);
    else // (sizeof(FT) == 8) 
      dat[i].x = (double) to_double(in_data);
    fread(&(in_data.x[0]), sizeof(double), 1, fid);
    fread(&(in_data.x[1]), sizeof(double), 1, fid);
    if (sizeof(FT) == 4) 
      dat[i].y = (float) to_double(in_data);
    else // (sizeof(FT) == 8) 
      dat[i].y = (double) to_double(in_data);
  }
  */
}

void outputData(FILE* outfile, FT outdat)
{
  dd_real out_data((double)outdat);
  cout << "out_data : " << out_data << endl;
  fwrite(&(out_data.x[0]), sizeof(double), 1, outfile);
  fwrite(&(out_data.x[1]), sizeof(double), 1, outfile);
}
  
__device__ void GPU_FFT2( FT2 &v1,FT2 &v2 ) { 
  FT2 v0 = v1;  
  v1 = v0 + v2; 
  v2 = v0 - v2; 
}

__device__ void GPU_FFT4( FT2 &v0,FT2 &v1,FT2 &v2,FT2 &v3) { 
   GPU_FFT2(v0, v2);
   GPU_FFT2(v1, v3);
   v3 = v3 * exp_1_4;
   GPU_FFT2(v0, v1);
   GPU_FFT2(v2, v3);    
}


inline __device__ void GPU_FFT2(FT2* v){
  GPU_FFT2(v[0],v[1]);
}

inline __device__ void GPU_FFT4(FT2* v){
  GPU_FFT4(v[0],v[1],v[2],v[3] );
}


inline __device__ void GPU_FFT8(FT2* v){
  GPU_FFT2(v[0],v[4]);
  GPU_FFT2(v[1],v[5]);
  GPU_FFT2(v[2],v[6]);
  GPU_FFT2(v[3],v[7]);

  v[5]=(v[5]*exp_1_8)*M_SQRT1_2;
  v[6]=v[6]*exp_1_4;
  v[7]=(v[7]*exp_3_8)*M_SQRT1_2;

  GPU_FFT4(v[0],v[1],v[2],v[3]);
  GPU_FFT4(v[4],v[5],v[6],v[7]);
  
}

inline __device__ void GPU_FFT16( FT2 *v )
{
    GPU_FFT4( v[0], v[4], v[8], v[12] );
    GPU_FFT4( v[1], v[5], v[9], v[13] );
    GPU_FFT4( v[2], v[6], v[10], v[14] );
    GPU_FFT4( v[3], v[7], v[11], v[15] );

    v[5]  = (v[5]  * exp_1_8 ) * M_SQRT1_2;
    v[6]  =  v[6]  * exp_1_4;
    v[7]  = (v[7]  * exp_3_8 ) * M_SQRT1_2;
    v[9]  =  v[9]  * exp_1_16;
    v[10] = (v[10] * exp_1_8 ) * M_SQRT1_2;
    v[11] =  v[11] * exp_3_16;
    v[13] =  v[13] * exp_3_16;
    v[14] = (v[14] * exp_3_8 ) * M_SQRT1_2;
    v[15] =  v[15] * exp_9_16;

    GPU_FFT4( v[0],  v[1],  v[2],  v[3] );
    GPU_FFT4( v[4],  v[5],  v[6],  v[7] );
    GPU_FFT4( v[8],  v[9],  v[10], v[11] );
    GPU_FFT4( v[12], v[13], v[14], v[15] );
}
     
__device__ int GPU_expand(int idxL, int N1, int N2 ){ 
  return (idxL/N1)*N1*N2 + (idxL%N1); 
}      

__device__ void GPU_FftIteration(int j, int Ns, FT2* data0, FT2* data1){ 
  FT2 v[R];  	
  int idxS = j;       
  FT angle = -2*M_PI*(j%Ns)/(Ns*R);      

  for( int r=0; r<R; r++ ) { 
    v[r] = data0[idxS+r*NN/R]; 
    v[r] = v[r]*make_FT2(cos(r*angle), sin(r*angle)); 
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

__global__ void GPU_FFT_Global(int Ns, FT2* data0, FT2* data1) { 
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

  // get IO file names
  assert(argc == 3);
  char *inname = argv[1]; 
  char *outname = argv[2];

  // int N, B;
  n_bytes = NN*BB*sizeof(FT2);

  cudaSetDevice(GPUID);
  
  FT2 *source;
  FT2 *result;
  cudaMallocHost((void**)&source, n_bytes);
  CUERR;
  cudaMallocHost((void**)&result, n_bytes);
  CUERR;

  FT2 *d_source, *d_work;
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

    inputData(infile,(FT2*)source,NN*BB);

    // copy host memory to device
    cudaMemcpy(d_source, source, n_bytes,cudaMemcpyHostToDevice);
    CUERR;
    cudaMemset(d_work, 0,n_bytes);
    CUERR;
    
    for( int Ns=1; Ns<NN; Ns*=R){
      GPU_FFT_Global<<<dim3(BB), dim3(NN/R)>>>(Ns, d_source, d_work);
      FT2 *tmp = d_source;
      d_source = d_work;
      d_work = tmp;
    }

    cudaMemcpy(result, d_source, n_bytes,cudaMemcpyDeviceToHost);
    CUERR;

    outputData(outfile, (FT)result[NN*BB-1].y);
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

