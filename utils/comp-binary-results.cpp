#include <stdio.h>
#include <stdlib.h> 
#include <assert.h>
#include <quadmath.h> 

using namespace std;


unsigned long
getFileSize (FILE *fd) {
  assert(fd != NULL);

  unsigned long pos_cur = ftell(fd); 

  fseek(fd, 0, SEEK_END);

  unsigned long pos_end = ftell(fd);

  fseek(fd, pos_cur, SEEK_SET);

  return pos_end;
}


void 
fillData (FILE *fd,
	  unsigned int bw,
	  unsigned long n_vals,
	  __float128 *data) {
  assert(fd != NULL);

  for (unsigned long i = 0 ; i < n_vals ; i++) {
    if (bw == 32) {
      float inv;
      fread(&inv, sizeof(float), 1, fd);
      data[i] = inv; 
    }
    else if (bw == 64) {
      double inv;
      fread(&inv, sizeof(double), 1, fd);
      data[i] = inv; 
    }
    else assert(false); 
  }
}



int main (int argc, char **argv) {
  unsigned int bw_0, bw_1;
  char *fname_0, *fname_1;
  FILE *file_0, *file_1;

  if (argc != 5) {
    printf("Usage: comp-binary-results 1st-file-bit-width 1st-file-name 2nd-file-bit-width 2nd-file-name\n");
    exit(1);
  }

  bw_0 = atoi(argv[1]);
  fname_0 = argv[2];
  
  bw_1 = atoi(argv[3]);
  fname_1 = argv[4];

  assert(bw_0 == 32 || bw_0 == 64);
  assert(bw_1 == 32 || bw_1 == 64);

  file_0 = fopen(fname_0, "r");
  file_1 = fopen(fname_1, "r");
  assert(file_0 != NULL && file_1 != NULL); 

  unsigned long fsize_0, fsize_1;
  fsize_0 = getFileSize(file_0);
  fsize_1 = getFileSize(file_1);

  assert(fsize_0 % (bw_0 / 8) == 0);
  assert(fsize_1 % (bw_1 / 8) == 0);
  assert((fsize_0 / (bw_0 / 8)) == (fsize_1 / (bw_1 / 8)));

  unsigned long n_vals = fsize_0 / (bw_0 / 8);

  __float128 *data_0 = (__float128*) malloc(sizeof(__float128) * n_vals);
  __float128 *data_1 = (__float128*) malloc(sizeof(__float128) * n_vals);

  fillData(file_0, bw_0, n_vals, data_0);
  fillData(file_1, bw_1, n_vals, data_1);

  fclose(file_0);
  fclose(file_1);

  __float128 max_diff = 0.0; 
  __float128 ave_diff = 0.0; 

  for (unsigned long i = 0 ; i < n_vals ; i++) {
    __float128 this_diff = data_0[i] - data_1[i];
    if (this_diff < 0) this_diff *= -1.0;

    if (this_diff > max_diff) max_diff = this_diff;

    ave_diff += this_diff; 
  }
  ave_diff /= (__float128)n_vals;

  __float128 var_diff = 0.0;
  for (unsigned int i = 0 ; i < n_vals ; i++) {
    __float128 this_diff = (data_0[i] - data_1[i]) - ave_diff;
    this_diff = this_diff * this_diff;

    var_diff += this_diff;
  }
  var_diff /= (__float128)n_vals; 

  printf("Max  diff. %f\n", (double)max_diff);
  printf("Ave. diff. %f\n", (double)ave_diff);
  printf("Var. diff. %f\n", (double)var_diff); 
  
  return 0; 
}
