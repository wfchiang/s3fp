#include <stdio.h>
#include <stdlib.h> 


int main (int argc, char **argv) {
  
  char *fname_0 = "__test_file_0";
  char *fname_1 = "__test_file_1";

  
  // ==== test comp-binary-results ====
  FILE *file_0 = fopen(fname_0, "w");
  FILE *file_1 = fopen(fname_1, "w");

  float  f32s[3] = {0.1, 0.2, 0.3}; 
  double f64s[3] = {0.3, 0.2, 0.1}; 

  fwrite(f32s, sizeof(float),  3, file_0);
  fwrite(f64s, sizeof(double), 3, file_1);

  fclose(file_0);
  fclose(file_1);

  printf("==== test comp-binary-results ====\n"); 
  system("./comp-binary-results 32 __test_file_0 64 __test_file_1"); 

  
  return 0; 
}
