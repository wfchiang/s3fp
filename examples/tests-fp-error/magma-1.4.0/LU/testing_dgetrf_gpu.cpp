/*
    -- MAGMA (version 1.4.0) --
       Univ. of Tennessee, Knoxville
       Univ. of California, Berkeley
       Univ. of Colorado, Denver
       August 2013

       @generated d Tue Aug 13 16:46:00 2013
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


double *original_A;


// Initialize matrix to random.
// Having this in separate function ensures the same ISEED is always used,
// so we can re-generate the identical matrix.
void init_matrix( int m, int n, double *h_A, magma_int_t lda )
{
    magma_int_t ione = 1;
    magma_int_t ISEED[4] = {0,0,0,1};
    magma_int_t n2 = lda*n;
    lapackf77_dlarnv( &ione, ISEED, &n2, h_A );
}


// On input, A and ipiv is LU factorization of A. On output, A is overwritten.
// Requires m == n.
// Uses init_matrix() to re-generate original A as needed.
// Generates random RHS b and solves Ax=b.
// Returns residual, |Ax - b| / (n |A| |x|).
double get_residual(
    magma_int_t m, magma_int_t n,
    double *A, magma_int_t lda,
    magma_int_t *ipiv )
{
    if ( m != n ) {
        printf( "\nERROR: residual check defined only for square matrices\n" );
        return -1;
    }
    
    const double c_one     = MAGMA_D_ONE;
    const double c_neg_one = MAGMA_D_NEG_ONE;
    const magma_int_t ione = 1;
    
    // this seed should be DIFFERENT than used in init_matrix
    // (else x is column of A, so residual can be exactly zero)
    magma_int_t ISEED[4] = {0,0,0,2};
    magma_int_t info = 0;
    double *x, *b;
    
    // initialize RHS
    TESTING_MALLOC( x, double, n );
    TESTING_MALLOC( b, double, n );
    lapackf77_dlarnv( &ione, ISEED, &n, b );
    blasf77_dcopy( &n, b, &ione, x, &ione );
    
    // solve Ax = b
    lapackf77_dgetrs( "Notrans", &n, &ione, A, &lda, ipiv, x, &n, &info );
    if (info != 0)
        printf("lapackf77_dgetrs returned error %d: %s.\n",
               (int) info, magma_strerror( info ));
    
    // reset to original A
    init_matrix( m, n, A, lda );
    
    // compute r = Ax - b, saved in b
    blasf77_dgemv( "Notrans", &m, &n, &c_one, A, &lda, x, &ione, &c_neg_one, b, &ione );
    
    // compute residual |Ax - b| / (n*|A|*|x|)
    double norm_x, norm_A, norm_r, work[1];
    norm_A = lapackf77_dlange( "F", &m, &n, A, &lda, work );
    norm_r = lapackf77_dlange( "F", &n, &ione, b, &n, work );
    norm_x = lapackf77_dlange( "F", &n, &ione, x, &n, work );
    
    //printf( "r=\n" ); magma_dprint( 1, n, b, 1 );
    
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
double get_LU_error(magma_int_t M, magma_int_t N,
                    double *LU, magma_int_t lda,
                    magma_int_t *ipiv)
{
    magma_int_t min_mn = min(M,N);
    magma_int_t ione   = 1;
    magma_int_t mone   = -1;
    magma_int_t i, j;
    double alpha = MAGMA_D_ONE;
    double beta  = MAGMA_D_ZERO;
    double *A, *L, *U;
    double work[1], matnorm, residual;
    
    TESTING_MALLOC( A, double, lda*N    );
    TESTING_MALLOC( L, double, M*min_mn );
    TESTING_MALLOC( U, double, min_mn*N );
    memset( L, 0, M*min_mn*sizeof(double) );
    memset( U, 0, min_mn*N*sizeof(double) );

    // set to original A
    // init_matrix( M, N, A, lda );
    // lapackf77_dlaswp( &N, A, &lda, &ione, &min_mn, ipiv, &ione);
    
    // copy LU to L and U, and set diagonal to 1
    lapackf77_dlacpy( MagmaLowerStr, &M, &min_mn, LU, &lda, L, &M      );
    lapackf77_dlacpy( MagmaUpperStr, &min_mn, &N, LU, &lda, U, &min_mn );
    for(j=0; j<min_mn; j++)
        L[j+j*M] = MAGMA_D_MAKE( 1., 0. );
    
    // matnorm = lapackf77_dlange("f", &M, &N, A, &lda, work);

    blasf77_dgemm("N", "N", &M, &N, &min_mn,
                  &alpha, L, &M, U, &min_mn, &beta, LU, &lda);

    lapackf77_dlaswp( &N, LU, &lda, &ione, &min_mn, ipiv, &mone);
    /*
    for (i = 0 ; i < M*N ; i++) 
      printf("LU[%d] vs A[%d] ==  %11.10f  vs  %11.10f \n", i, i, LU[i], original_A[i]);
    */

    /*
    for( j = 0; j < N; j++ ) {
        for( i = 0; i < M; i++ ) {
            LU[i+j*lda] = MAGMA_D_SUB( LU[i+j*lda], A[i+j*lda] );
        }
    }
    residual = lapackf77_dlange("f", &M, &N, LU, &lda, work);
    */
    TESTING_FREE(A);
    TESTING_FREE(L);
    TESTING_FREE(U);

    // return residual / (matnorm * N);
    return LU[M*N-1];
}


/* ////////////////////////////////////////////////////////////////////////////
   -- Testing dgetrf
*/
int main( int argc, char** argv)
{
    TESTING_INIT();

    real_Double_t   gflops, gpu_perf, gpu_time, cpu_perf=0, cpu_time=0;
    double          error;
    double *h_A;
    double *d_A;
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

    double tol = opts.tolerance * lapackf77_dlamch("E");
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
            gflops = FLOPS_DGETRF( M, N ) / 1e9;
            
            TESTING_MALLOC(    ipiv, magma_int_t,     min_mn );
            TESTING_MALLOC(    h_A,  double, n2     );
            TESTING_DEVALLOC(  d_A,  double, ldda*N );
	    TESTING_MALLOC(original_A, double, n2);
            
            /* =====================================================================
               Performs operation using LAPACK
               =================================================================== */
            if ( opts.lapack ) {
                init_matrix( M, N, h_A, lda );
                
                cpu_time = magma_wtime();
                lapackf77_dgetrf(&M, &N, h_A, &lda, ipiv, &info);
                cpu_time = magma_wtime() - cpu_time;
                cpu_perf = gflops / cpu_time;
                if (info != 0)
                    printf("lapackf77_dgetrf returned error %d: %s.\n",
                           (int) info, magma_strerror( info ));
            }
            
            /* ====================================================================
               Performs operation using MAGMA
               =================================================================== */
            init_matrix( M, N, h_A, lda );

	    // buck testing 
	    for (unsigned int bi = 0 ; bi < N_REPEATS ; bi++) {

	    // overwrite 
	    fillInputs32to64(infile, h_A, lda*N);
	    memcpy(original_A, h_A, lda*N*sizeof(double));

            magma_dsetmatrix( M, N, h_A, lda, d_A, ldda );
            
            gpu_time = magma_wtime();
            magma_dgetrf_gpu( M, N, d_A, ldda, ipiv, &info);
            gpu_time = magma_wtime() - gpu_time;
            gpu_perf = gflops / gpu_time;
            if (info != 0)
                printf("magma_dgetrf_gpu returned error %d: %s.\n",
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
                magma_dgetmatrix( M, N, d_A, ldda, h_A, lda );
                error = get_residual( M, N, h_A, lda, ipiv );
                printf("   %8.2e%s\n", error, (error < tol ? "" : "  failed"));
                status |= ! (error < tol);
            }
            else if ( opts.check ) {
                magma_dgetmatrix( M, N, d_A, ldda, h_A, lda );
                error = get_LU_error( M, N, h_A, lda, ipiv );
                printf("   %8.2e%s\n", error, (error < tol ? "" : "  failed"));
                status |= ! (error < tol);
            }
            else {
                printf("     ---  \n");
            }
	    */
	    // copy back 
	    magma_dgetmatrix( M, N, d_A, ldda, h_A, lda );

	    // check error 
	    double value_to_check = get_LU_error( M, N, h_A, lda, ipiv );
	    writeOutput64to128(outfile, value_to_check);

	    // write output 
	    // writeOutput64(outfile, h_A[(lda*N-N)-1]);
	    // writeOutput64(outfile, h_A[n2-1]);
	    // writeOutput64(outfile, h_A[(n2-lda)-1]);
	    // printf("dgetrf output (%d) %11.10f\n", bi, h_A[n2-1]);

	    } // the end of the buck testing .... 

            
            TESTING_FREE( ipiv );
            TESTING_FREE( h_A );
	    TESTING_FREE( original_A );
            TESTING_DEVFREE( d_A );
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
