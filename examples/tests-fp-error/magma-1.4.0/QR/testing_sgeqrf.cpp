/*
    -- MAGMA (version 1.4.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       August 2013

       @generated s Tue Aug 13 16:46:07 2013
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

extern "C" {
#include <quadmath.h>
}

#define N_REPEATS 10

/* ////////////////////////////////////////////////////////////////////////////
   -- Testing sgeqrf
*/
int main( int argc, char** argv)
{
    TESTING_INIT();

    real_Double_t    gflops, gpu_perf, gpu_time, cpu_perf=0, cpu_time=0;
    float           error, work[1];
    float  c_neg_one = MAGMA_S_NEG_ONE;
    float *h_A, *h_R, *tau, *h_work, tmp[1];
    magma_int_t M, N, n2, lda, lwork, info, min_mn, nb;
    magma_int_t ione     = 1;
    magma_int_t ISEED[4] = {0,0,0,1}, ISEED2[4];

    /* my hackings on arguments*/
    assert(argc >= 3);
    char *inname = argv[argc-2];
    char *outname = argv[argc-1];
    argv[argc-2] = NULL;
    argv[argc-1] = NULL;
    argc -= 2;
    FILE *infile = fopen(inname, "r");
    FILE *outfile = fopen(outname, "w");
    assert(infile != NULL);
    assert(outfile != NULL);
    
    magma_opts opts;
    parse_opts( argc, argv, &opts );

    /* my hackings on opts */
    opts.ntest = 1;
    opts.niter = 1;
    // opts.lapack = 1;
    opts.check = 0;

    magma_int_t status = 0;
    float tol, eps = lapackf77_slamch("E");
    tol = opts.tolerance * eps;

    opts.lapack |= ( opts.check == 2 );  // check (-c2) implies lapack (-l)

    printf("ngpu %d\n", (int) opts.ngpu );
    if ( opts.check == 1 ) {
        printf("  M     N     CPU GFlop/s (sec)   GPU GFlop/s (sec)   ||R-Q'A||_1 / (M*||A||_1) ||I-Q'Q||_1 / M\n");
        printf("===============================================================================================\n");
    } else {
        printf("  M     N     CPU GFlop/s (sec)   GPU GFlop/s (sec)   ||R||_F / ||A||_F\n");
        printf("=======================================================================\n");
    }
    for( int i = 0; i < opts.ntest; ++i ) {
        for( int iter = 0; iter < opts.niter; ++iter ) {
            M = opts.msize[i];
            N = opts.nsize[i];

	    /* my assertion */
	    assert(M == N);

            min_mn = min(M, N);
            lda    = M;
            n2     = lda*N;
            nb     = magma_get_sgeqrf_nb(M);
            gflops = FLOPS_SGEQRF( M, N ) / 1e9;
            
            lwork = -1;
            lapackf77_sgeqrf(&M, &N, h_A, &M, tau, tmp, &lwork, &info);
            lwork = (magma_int_t)MAGMA_S_REAL( tmp[0] );
            lwork = max( lwork, max( N*nb, 2*nb*nb ));
            
            TESTING_MALLOC(    tau, float, min_mn );
            TESTING_MALLOC(    h_A, float, n2     );
            TESTING_HOSTALLOC( h_R, float, n2     );
            TESTING_MALLOC( h_work, float, lwork );

	    for (unsigned int r = 0 ; r < N_REPEATS ; r++) { // for repeats 
            
            /* Initialize the matrix */
            for ( int j=0; j<4; j++ ) ISEED2[j] = ISEED[j]; // saving seeds
            lapackf77_slarnv( &ione, ISEED, &n2, h_A );

	    /* My initialize data */
	    for (unsigned int ni = 0 ; ni < n2 ; ni++) {
	      __float128 idata; 
	      fread(&idata, sizeof(__float128), 1, infile);
	      h_A[ni] = (float) idata;
	      if(ni == 0)
		printf("%GD 11.10f  (%11.10f) \n", h_A[ni], (float) idata);
	      assert(h_A[ni] == idata);
	    }

            /* ====================================================================
               Performs operation using MAGMA
               =================================================================== */

            if ( opts.lapack ) {
	      /* =====================================================================
		 Performs operation using LAPACK
		 =================================================================== */
	      float *tau;
	      TESTING_MALLOC( tau, float, min_mn );
	      cpu_time = magma_wtime();
	      lapackf77_sgeqrf(&M, &N, h_A, &lda, tau, h_work, &lwork, &info);
	      cpu_time = magma_wtime() - cpu_time;
	      cpu_perf = gflops / cpu_time;
	      if (info != 0)
		printf("lapackf77_sgeqrf returned error %d: %s.\n",
		       (int) info, magma_strerror( info ));
	      TESTING_FREE( tau );
	      
	      // output 
	      printf("LAPACK : %11.10f \n", h_A[n2/2]);
	      __float128 odata = h_A[n2-1];
	      fwrite(&odata, sizeof(__float128), 1, outfile);
            }
	    /* go either MAGMA or LAPACK */
	    else {
	      lapackf77_slacpy( MagmaUpperLowerStr, &M, &N, h_A, &lda, h_R, &lda );

	      gpu_time = magma_wtime();
	      magma_sgeqrf(M, N, h_R, lda, tau, h_work, lwork, &info);
	      gpu_time = magma_wtime() - gpu_time;
	      gpu_perf = gflops / gpu_time;
	      if (info != 0)
                printf("magma_sgeqrf returned error %d: %s.\n",
                       (int) info, magma_strerror( info ));
	      
	      // output 
	      printf("MAGMA : %11.10f \n", h_R[n2-1]);
	      __float128 odata = h_R[n2-1];
	      fwrite(&odata, sizeof(__float128), 1, outfile);
	    }

	    } // for repeats 

            if ( opts.check == 1 ) {
                /* =====================================================================
                   Check the result 
                   =================================================================== */
	      /*
                magma_int_t lwork = n2+N;
                float *h_W1, *h_W2, *h_W3;
                float *h_RW, results[2];

                TESTING_MALLOC( h_W1, float, n2 ); // Q
                TESTING_MALLOC( h_W2, float, n2 ); // R
                TESTING_MALLOC( h_W3, float, lwork ); // WORK
                TESTING_MALLOC( h_RW, float, M );  // RWORK
                lapackf77_slarnv( &ione, ISEED2, &n2, h_A );
                lapackf77_sqrt02( &M, &N, &min_mn, h_A, h_R, h_W1, h_W2, &lda, tau, h_W3, &lwork,
                                  h_RW, results );
                results[0] *= eps;
                results[1] *= eps;

                if ( opts.lapack ) {
                    printf("%5d %5d   %7.2f (%7.2f)   %7.2f (%7.2f)   %8.2e                  %8.2e",
                           (int) M, (int) N, cpu_perf, cpu_time, gpu_perf, gpu_time, results[0],results[1] );
                    printf("%s\n", (results[0] < tol ? "" : "  failed"));
                } else {
                    printf("%5d %5d     ---   (  ---  )   %7.2f (%7.2f)    %8.2e                  %8.2e",
                           (int) M, (int) N, gpu_perf, gpu_time, results[0],results[1] );
                    printf("%s\n", (results[0] < tol ? "" : "  failed"));
                }
                status |= ! (results[0] < tol);

                TESTING_FREE( h_W1 );
                TESTING_FREE( h_W2 );
                TESTING_FREE( h_W3 );
                TESTING_FREE( h_RW );
	      */
            } else if ( opts.check == 2 ) {
                /* =====================================================================
                   Check the result compared to LAPACK
                   =================================================================== */
	      /*
                error = lapackf77_slange("f", &M, &N, h_A, &lda, work);
                blasf77_saxpy(&n2, &c_neg_one, h_A, &ione, h_R, &ione);
                error = lapackf77_slange("f", &M, &N, h_R, &lda, work) / error;
                
                if ( opts.lapack ) {
                    printf("%5d %5d   %7.2f (%7.2f)   %7.2f (%7.2f)   %8.2e",
                           (int) M, (int) N, cpu_perf, cpu_time, gpu_perf, gpu_time, error );
                } else {
                    printf("%5d %5d     ---   (  ---  )   %7.2f (%7.2f)    %8.2e",
                           (int) M, (int) N, gpu_perf, gpu_time, error );
                }
                printf("%s\n", (error < tol ? "" : "  failed"));
                status |= ! (error < tol);
	      */
            }
            else {
	      /*
                if ( opts.lapack ) {
                    printf("%5d %5d   %7.2f (%7.2f)   %7.2f (%7.2f)   ---\n",
                           (int) M, (int) N, cpu_perf, cpu_time, gpu_perf, gpu_time );
                } else {
                    printf("%5d %5d     ---   (  ---  )   %7.2f (%7.2f)     ---  \n",
                           (int) M, (int) N, gpu_perf, gpu_time);
                }
	      */
            }
            
            TESTING_FREE( tau );
            TESTING_FREE( h_A );
            TESTING_FREE( h_work );
            TESTING_HOSTFREE( h_R );
        }
	    /*
        if ( opts.niter > 1 ) {
            printf( "\n" );
        }
	    */
    }

    TESTING_FINALIZE();

    /* finalize */
    fclose(infile);
    fclose(outfile);

    return status;
}
