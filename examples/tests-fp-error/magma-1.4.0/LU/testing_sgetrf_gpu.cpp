/*
    -- MAGMA (version 1.4.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       August 2013

       @generated s Wed Aug 14 12:18:04 2013
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


float *original_A;


// Initialize matrix to random.
// Having this in separate function ensures the same ISEED is always used,
// so we can re-generate the identical matrix.
void init_matrix( int m, int n, float *h_A, magma_int_t lda )
{
    magma_int_t ione = 1;
    magma_int_t ISEED[4] = {0,0,0,1};
    magma_int_t n2 = lda*n;
    lapackf77_slarnv( &ione, ISEED, &n2, h_A );
}


// On input, A and ipiv is LU factorization of A. On output, A is overwritten.
// Requires m == n.
// Uses init_matrix() to re-generate original A as needed.
// Generates random RHS b and solves Ax=b.
// Returns residual, |Ax - b| / (n |A| |x|).
float get_residual(
    magma_int_t m, magma_int_t n,
    float *A, magma_int_t lda,
    magma_int_t *ipiv )
{
    if ( m != n ) {
        printf( "\nERROR: residual check defined only for square matrices\n" );
        return -1;
    }
    
    const float c_one     = MAGMA_S_ONE;
    const float c_neg_one = MAGMA_S_NEG_ONE;
    const magma_int_t ione = 1;
    
    // this seed should be DIFFERENT than used in init_matrix
    // (else x is column of A, so residual can be exactly zero)
    magma_int_t ISEED[4] = {0,0,0,2};
    magma_int_t info = 0;
    float *x, *b;
    
    // initialize RHS
    TESTING_MALLOC( x, float, n );
    TESTING_MALLOC( b, float, n );
    lapackf77_slarnv( &ione, ISEED, &n, b );
    blasf77_scopy( &n, b, &ione, x, &ione );
    
    // solve Ax = b
    lapackf77_sgetrs( "Notrans", &n, &ione, A, &lda, ipiv, x, &n, &info );
    if (info != 0)
        printf("lapackf77_sgetrs returned error %d: %s.\n",
               (int) info, magma_strerror( info ));
    
    // reset to original A
    init_matrix( m, n, A, lda );
    
    // compute r = Ax - b, saved in b
    blasf77_sgemv( "Notrans", &m, &n, &c_one, A, &lda, x, &ione, &c_neg_one, b, &ione );
    
    // compute residual |Ax - b| / (n*|A|*|x|)
    float norm_x, norm_A, norm_r, work[1];
    norm_A = lapackf77_slange( "F", &m, &n, A, &lda, work );
    norm_r = lapackf77_slange( "F", &n, &ione, b, &n, work );
    norm_x = lapackf77_slange( "F", &n, &ione, x, &n, work );
    
    //printf( "r=\n" ); magma_sprint( 1, n, b, 1 );
    
    TESTING_FREE( x );
    TESTING_FREE( b );
    
    //printf( "r=%.2e, A=%.2e, x=%.2e, n=%d\n", norm_r, norm_A, norm_x, n );
    return norm_r / (n * norm_A * norm_x);
}


// On input, LU and ipiv is LU factorization of A. On output, LU is overwritten.
// Works for any m, n.
// Uses init_matrix() to re-generate original A as needed.
// Returns error in factorization, |PA - LU| / (n |A|)
// This allocates 3 more matrices to store A, L, and U.
// NOTE : WFCHIANG: the return value was changed to the value for error checking 
float get_LU_error(magma_int_t M, magma_int_t N,
                    float *LU, magma_int_t lda,
                    magma_int_t *ipiv)
{
    magma_int_t min_mn = min(M,N);
    magma_int_t ione   = 1;
    magma_int_t mone   = -1;
    magma_int_t i, j;
    float alpha = MAGMA_S_ONE;
    float beta  = MAGMA_S_ZERO;
    float *A, *L, *U;
    float work[1], matnorm, residual;
    
    TESTING_MALLOC( A, float, lda*N    );
    TESTING_MALLOC( L, float, M*min_mn );
    TESTING_MALLOC( U, float, min_mn*N );
    memset( L, 0, M*min_mn*sizeof(float) );
    memset( U, 0, min_mn*N*sizeof(float) );

    // set to original A
    // init_matrix( M, N, A, lda );

    // lapackf77_slaswp( &N, A, &lda, &ione, &min_mn, ipiv, &ione);
    
    // copy LU to L and U, and set diagonal to 1
    lapackf77_slacpy( MagmaLowerStr, &M, &min_mn, LU, &lda, L, &M      );
    lapackf77_slacpy( MagmaUpperStr, &min_mn, &N, LU, &lda, U, &min_mn );
    for(j=0; j<min_mn; j++)
        L[j+j*M] = MAGMA_S_MAKE( 1., 0. );
    
    // matnorm = lapackf77_slange("f", &M, &N, A, &lda, work);

    blasf77_sgemm("N", "N", &M, &N, &min_mn,
                  &alpha, L, &M, U, &min_mn, &beta, LU, &lda);

    lapackf77_slaswp( &N, LU, &lda, &ione, &min_mn, ipiv, &mone);
    /*
    for (i = 0 ; i < M*N ; i++) 
      printf("LU[%d] vs A[%d] ==  %11.10f  vs  %11.10f \n", i, i, LU[i], original_A[i]);
    */

    /*
    for( j = 0; j < N; j++ ) {
        for( i = 0; i < M; i++ ) {
            LU[i+j*lda] = MAGMA_S_SUB( LU[i+j*lda], A[i+j*lda] );
        }
    }
    residual = lapackf77_slange("f", &M, &N, LU, &lda, work);
    */
    TESTING_FREE(A);
    TESTING_FREE(L);
    TESTING_FREE(U);

    // return residual / (matnorm * N);
    return LU[M*N-1];
}


/* ////////////////////////////////////////////////////////////////////////////
   -- Testing sgetrf
*/
int main( int argc, char** argv)
{
    TESTING_INIT();

    real_Double_t   gflops, gpu_perf, gpu_time, cpu_perf=0, cpu_time=0;
    float          error;
    float *h_A;
    float *d_A;
    magma_int_t     *ipiv;
    magma_int_t M, N, n2, lda, ldda, info, min_mn;
    magma_int_t status   = 0;

    // hack the arguments
    char *input_name = argv[argc-2];
    char *output_name = argv[argc-1];
    assert(argc >= 7);
    argv[argc-1] = NULL;
    argv[argc-2] = NULL;
    argc -= 2;

    magma_opts opts;
    parse_opts( argc, argv, &opts );

    float tol = opts.tolerance * lapackf77_slamch("E");

    /*    
    if ( opts.check == 2 ) {
        printf("    M     N   CPU GFlop/s (sec)   GPU GFlop/s (sec)   |Ax-b|/(N*|A|*|x|)\n");
    }
    else {
        printf("    M     N   CPU GFlop/s (sec)   GPU GFlop/s (sec)   |PA-LU|/(N*|A|)\n");
    }
    printf("=========================================================================\n");
    */
    FILE *infile = fopen(input_name, "r");
    assert(infile != NULL);
    FILE *outfile = fopen(output_name, "w");
    assert(outfile != NULL);

    for (int i = 0 ; i < 1 ; i++) {
      for (int iter = 0 ; iter < 1 ; iter++) {
    /*
    for( int i = 0; i < opts.ntest; ++i ) {
        for( int iter = 0; iter < opts.niter; ++iter ) {
    */
            M = opts.msize[i];
            N = opts.nsize[i];
            min_mn = min(M, N);
            lda    = M;
            n2     = lda*N;
            ldda   = ((M+31)/32)*32;
            gflops = FLOPS_SGETRF( M, N ) / 1e9;
            
            TESTING_MALLOC(    ipiv, magma_int_t,     min_mn );
            TESTING_MALLOC(    h_A,  float, n2     );
            TESTING_DEVALLOC(  d_A,  float, ldda*N );
	    TESTING_MALLOC(original_A, float, n2);

            /* =====================================================================
               Performs operation using LAPACK
               =================================================================== */
            if ( opts.lapack ) {
                init_matrix( M, N, h_A, lda );
                
                cpu_time = magma_wtime();
                lapackf77_sgetrf(&M, &N, h_A, &lda, ipiv, &info);
                cpu_time = magma_wtime() - cpu_time;
                cpu_perf = gflops / cpu_time;
                if (info != 0)
                    printf("lapackf77_sgetrf returned error %d: %s.\n",
                           (int) info, magma_strerror( info ));
            }

            /* ====================================================================
               Performs operation using MAGMA
               =================================================================== */
            init_matrix( M, N, h_A, lda );
	    /*
	    float mina = h_A[0];
	    float maxa = h_A[0];
	    for (unsigned ii = 1 ; ii < lda*N ; ii++) {
	      if (h_A[ii] > maxa) maxa = h_A[ii];
	      if (h_A[ii] < mina) mina = h_A[ii];
	    }
	    printf("min %11.10f\n", mina);
	    printf("max %11.10f\n", maxa);
	    */

	    // buck testing 
	    for (int bi = 0 ; bi < N_REPEATS ; bi++) {

	    // overwrite 
	    fillInputs32(infile, h_A, lda*N);	    
	    memcpy(original_A, h_A, lda*N*sizeof(float));

            magma_ssetmatrix( M, N, h_A, lda, d_A, ldda );

            gpu_time = magma_wtime();
            magma_sgetrf_gpu( M, N, d_A, ldda, ipiv, &info);
            gpu_time = magma_wtime() - gpu_time;
            gpu_perf = gflops / gpu_time;
            if (info != 0)
                printf("magma_sgetrf_gpu returned error %d: %s.\n",
                       (int) info, magma_strerror( info ));

            /* =====================================================================
               Check the factorization
               =================================================================== */
	    /*
            if ( opts.lapack ) {
                printf("%5d %5d   %7.2f (%7.2f)   %7.2f (%7.2f)",
                       (int) M, (int) N, cpu_perf, cpu_time, gpu_perf, gpu_time );
            }
            else {
                printf("%5d %5d     ---   (  ---  )   %7.2f (%7.2f)",
                       (int) M, (int) N, gpu_perf, gpu_time );
            }
	    */
	    /*
            if ( opts.check == 2 ) {
                magma_sgetmatrix( M, N, d_A, ldda, h_A, lda );
                error = get_residual( M, N, h_A, lda, ipiv );
                printf("   %8.2e%s\n", error, (error < tol ? "" : "  failed"));
                status |= ! (error < tol);
            }
            else if ( opts.check ) {
                magma_sgetmatrix( M, N, d_A, ldda, h_A, lda );
                error = get_LU_error( M, N, h_A, lda, ipiv );
                printf("   %8.2e%s\n", error, (error < tol ? "" : "  failed"));
                status |= ! (error < tol);
            }
            else {
                printf("     ---  \n");
            }
	    */
	    // copy back 
	    magma_sgetmatrix( M, N, d_A, ldda, h_A, lda );

	    // check error
	    float value_to_check = get_LU_error( M, N, h_A, lda, ipiv );
	    writeOutput32(outfile, value_to_check);

	    // write output 
	    // writeOutput32(outfile, h_A[(lda*N-N)-1]);
	    // writeOutput32(outfile, h_A[n2-1]);
	    // writeOutput32(outfile, h_A[(n2-lda)-1]);
	    // printf("sgetrf output (%d) %11.10f\n", bi, h_A[n2-1]);
	    /*
	    for (int mi = 0 ; mi < M ; mi++) {
	      for (int ni = 0 ; ni < N ; ni++) {
		printf("h_A[%d][%d] = %11.10f\n", mi, ni, h_A[mi*M+ni]);
	      }
	    }
	    printf("============ \n");
	    */
	    } // the end of the buck test.... 

            TESTING_FREE( ipiv );
            TESTING_FREE( h_A );
            TESTING_DEVFREE( d_A );
	    TESTING_FREE(original_A);
        }
        if ( opts.niter > 1 ) {
            printf( "\n" );
        }
    }

    fclose(infile);
    fclose(outfile);

    TESTING_FINALIZE();
    return status;
}