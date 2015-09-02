/*
    -- MAGMA (version 1.4.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       August 2013

       @generated d Wed Aug 14 12:17:56 2013
       @author Mark Gates
*/
// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cuda_runtime_api.h>
#include <cublas.h>

// includes, project
#include "flops.h"
#include "magma.h"
#include "magma_lapack.h"
#include "testings.h"

#include <assert.h>
#include "dataio.h"

// #define N_REPEATS 1
#define N_REPEATS 10


// #define TAINED

/* ////////////////////////////////////////////////////////////////////////////
   -- Testing dgemm
*/
int main( int argc, char** argv)
{
    TESTING_INIT();

    real_Double_t   gflops, magma_perf, magma_time, cublas_perf, cublas_time, cpu_perf, cpu_time;
    double          magma_error, cublas_error, Cnorm, work[1];
    magma_int_t M, N, K;
    magma_int_t Am, An, Bm, Bn;
    magma_int_t sizeA, sizeB, sizeC;
    magma_int_t lda, ldb, ldc, ldda, lddb, lddc;
    magma_int_t ione     = 1;
    magma_int_t ISEED[4] = {0,0,0,1};
    
    double *h_A, *h_B, *h_C, *h_Cmagma, *h_Ccublas;
    double *d_A, *d_B, *d_C;
    double c_neg_one = MAGMA_D_NEG_ONE;
    double alpha = MAGMA_D_MAKE(  0.29, -0.86 );
    double beta  = MAGMA_D_MAKE( -0.48,  0.38 );

    // hack the arguments
    char *input_name = argv[argc-2];
    char *output_name = argv[argc-1];
    assert(argc >= 7);
    argv[argc-1] = NULL;
    argv[argc-2] = NULL;
    argc -= 2;
    
    magma_opts opts;
    parse_opts( argc, argv, &opts );
    /*    
    printf("If running lapack (option --lapack), MAGMA and CUBLAS error are both computed\n"
           "relative to CPU BLAS result. Else, MAGMA error is computed relative to CUBLAS result.\n\n"
           "transA = %c, transB = %c\n", opts.transA, opts.transB );
    printf("    M     N     K   MAGMA Gflop/s (ms)  CUBLAS Gflop/s (ms)   CPU Gflop/s (ms)  MAGMA error  CUBLAS error\n");
    printf("=========================================================================================================\n");
    */

    FILE *infile = fopen(input_name, "r"); 
    assert(infile != NULL);
    FILE *outfile = fopen(output_name, "w");
    assert(outfile != NULL);
    
    for( int i = 0; i < 1 ; ++i ) {
        for( int iter = 0; iter < 1 ; ++iter ) {
	  /*
    for( int i = 0; i < opts.ntest; ++i ) {
        for( int iter = 0; iter < opts.niter; ++iter ) {
	  */
            M = opts.msize[i];
            N = opts.nsize[i];
            K = opts.ksize[i];
            gflops = FLOPS_DGEMM( M, N, K ) / 1e9;

            if ( opts.transA == MagmaNoTrans ) {
                lda = Am = M;
                An = K;
            } else {
                lda = Am = K;
                An = M;
            }
            
            if ( opts.transB == MagmaNoTrans ) {
                ldb = Bm = K;
                Bn = N;
            } else {
                ldb = Bm = N;
                Bn = K;
            }
            ldc = M;
            
            ldda = ((lda+31)/32)*32;
            lddb = ((ldb+31)/32)*32;
            lddc = ((ldc+31)/32)*32;
            
            sizeA = lda*An;
            sizeB = ldb*Bn;
            sizeC = ldc*N;
            
            TESTING_MALLOC( h_A,  double, lda*An );
            TESTING_MALLOC( h_B,  double, ldb*Bn );
            TESTING_MALLOC( h_C,  double, ldc*N  );
            TESTING_MALLOC( h_Cmagma,  double, ldc*N  );
            TESTING_MALLOC( h_Ccublas, double, ldc*N  );
            
            TESTING_DEVALLOC( d_A, double, ldda*An );
            TESTING_DEVALLOC( d_B, double, lddb*Bn );
            TESTING_DEVALLOC( d_C, double, lddc*N  );
            
            /* Initialize the matrices */
            lapackf77_dlarnv( &ione, ISEED, &sizeA, h_A );
            lapackf77_dlarnv( &ione, ISEED, &sizeB, h_B );
            lapackf77_dlarnv( &ione, ISEED, &sizeC, h_C );

	    // buck test
	    for (unsigned int bi = 0 ; bi < N_REPEATS ; bi++) {
	      
	    // overwrite 
#ifdef TAINED
	    fillInputs64(infile, &alpha, 1);
	    // alpha = (double) 1.1;
	    fillInputs64(infile, &beta,  1);
	    // beta = (double) 1.2;
	    for (unsigned int ri = 0 ; ri < sizeA ; ri++) {
	      if (ri % lda == 0) {
		fillInputs64(infile, &h_A[ri], 1);
		// h_A[ri] = (double) ri;
	      }
	      else h_A[ri] = (double) 0.0;
	    }
	    for (unsigned int ci = 0 ; ci < sizeB ; ci++) {
	      if (ci < ldb) {
		fillInputs64(infile, &h_B[ci], 1);
		// h_B[ci] = (double) ci;
	      }
	      else h_B[ci] = (double) 0.0;
	    }
	    fillInputs64(infile, &h_C[0], 1);
	    // h_C[0] = (double) 1.3;
	    for (unsigned int rci = 1 ; rci < sizeC ; rci++) {
	      h_C[rci] = (double) 0.0;
	    }
#else
	    fillInputs64(infile, &alpha, 1);
	    fillInputs64(infile, &beta,  1);
	    fillInputs64(infile, h_A, sizeA);
	    fillInputs64(infile, h_B, sizeB);
	    fillInputs64(infile, h_C, sizeC);
#endif 
            
            /* =====================================================================
               Performs operation using MAGMA-BLAS
               =================================================================== */
            magma_dsetmatrix( Am, An, h_A, lda, d_A, ldda );
            magma_dsetmatrix( Bm, Bn, h_B, ldb, d_B, lddb );
            magma_dsetmatrix( M, N, h_C, ldc, d_C, lddc );
            
            magma_time = magma_sync_wtime( NULL );
            magmablas_dgemm( opts.transA, opts.transB, M, N, K,
                             alpha, d_A, ldda,
                                    d_B, lddb,
                             beta,  d_C, lddc );
            magma_time = magma_sync_wtime( NULL ) - magma_time;
            magma_perf = gflops / magma_time;
            
            magma_dgetmatrix( M, N, d_C, lddc, h_Cmagma, ldc );
            
            /* =====================================================================
               Performs operation using CUDA-BLAS
               =================================================================== */
	    /*
            magma_dsetmatrix( M, N, h_C, ldc, d_C, lddc );
            
            cublas_time = magma_sync_wtime( NULL );
            cublasDgemm( opts.transA, opts.transB, M, N, K,
                         alpha, d_A, ldda,
                                d_B, lddb,
                         beta,  d_C, lddc );
            cublas_time = magma_sync_wtime( NULL ) - cublas_time;
            cublas_perf = gflops / cublas_time;
            
            magma_dgetmatrix( M, N, d_C, lddc, h_Ccublas, ldc );
            */
            /* =====================================================================
               Performs operation using CPU BLAS
               =================================================================== */
	    /*
            if ( opts.lapack ) {
                cpu_time = magma_wtime();
                blasf77_dgemm( &opts.transA, &opts.transB, &M, &N, &K,
                               &alpha, h_A, &lda,
                                       h_B, &ldb,
                               &beta,  h_C, &ldc );
                cpu_time = magma_wtime() - cpu_time;
                cpu_perf = gflops / cpu_time;
            }
            */
            /* =====================================================================
               Check the result
               =================================================================== */
	    /*
            if ( opts.lapack ) {
                // compute relative error for both magma & cublas, relative to lapack,
                // |C_magma - C_lapack| / |C_lapack|
                Cnorm = lapackf77_dlange( "M", &M, &N, h_C, &ldc, work );
                
                blasf77_daxpy( &sizeC, &c_neg_one, h_C, &ione, h_Cmagma, &ione );
                magma_error = lapackf77_dlange( "M", &M, &N, h_Cmagma, &ldc, work ) / Cnorm;
                
                blasf77_daxpy( &sizeC, &c_neg_one, h_C, &ione, h_Ccublas, &ione );
                cublas_error = lapackf77_dlange( "M", &M, &N, h_Ccublas, &ldc, work ) / Cnorm;
                
                printf("%5d %5d %5d   %7.2f (%7.2f)    %7.2f (%7.2f)   %7.2f (%7.2f)    %8.2e     %8.2e\n",
                       (int) M, (int) N, (int) K,
                       magma_perf,  1000.*magma_time,
                       cublas_perf, 1000.*cublas_time,
                       cpu_perf,    1000.*cpu_time,
                       magma_error, cublas_error );
            }
            else {
                // compute relative error for magma, relative to cublas
                Cnorm = lapackf77_dlange( "M", &M, &N, h_Ccublas, &ldc, work );
                
                blasf77_daxpy( &sizeC, &c_neg_one, h_Ccublas, &ione, h_Cmagma, &ione );
                magma_error = lapackf77_dlange( "M", &M, &N, h_Cmagma, &ldc, work ) / Cnorm;
                
                printf("%5d %5d %5d   %7.2f (%7.2f)    %7.2f (%7.2f)     ---   (  ---  )    %8.2e     ---\n",
                       (int) M, (int) N, (int) K,
                       magma_perf,  1000.*magma_time,
                       cublas_perf, 1000.*cublas_time,
                       magma_error );
            }
	    */

	    // write output 
	    writeOutput64(outfile, h_Cmagma[0]);
	    printf("h_Cmagma[0] = %21.20f\n", h_Cmagma[0]);

	    } // the end of the buck testing 
            
            TESTING_FREE( h_A  );
            TESTING_FREE( h_B  );
            TESTING_FREE( h_C  );
            TESTING_FREE( h_Cmagma  );
            TESTING_FREE( h_Ccublas );
            
            TESTING_DEVFREE( d_A );
            TESTING_DEVFREE( d_B );
            TESTING_DEVFREE( d_C );
        }
        if ( opts.niter > 1 ) {
            printf( "\n" );
        }
    }

    fclose(infile);
    fclose(outfile);

    TESTING_FINALIZE();
    return 0;
}
