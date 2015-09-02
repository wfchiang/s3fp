#include <stdio.h>
#include <assert.h>

extern "C" {
#include <quadmath.h>
} 

#ifndef IFT
#define IFT
#endif 


int main (int argc, char *argv[]) {
  assert(argc == 2); 
  long n; 
  char *outname = argv[1]; 
  assert(outname != NULL); 
  FILE *outfile = fopen(outname, "w"); 
  assert(outfile != NULL); 
  
  printf("# points: ");
  scanf("%ld", &n); 

  for (long i = 0 ; i < n ; i++) {
    long double px, py; 
    IFT outx, outy; 
    printf("point [%ld]: ", i); 
    scanf("%Lf %Lf", &px, &py); 
    outx = px; 
    outy = py; 
    fwrite(&outx, sizeof(IFT), 1, outfile); 
    fwrite(&outy, sizeof(IFT), 1, outfile); 
  }

  return 0; 
}
