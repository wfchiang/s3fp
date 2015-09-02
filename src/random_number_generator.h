#include <stdlib.h>
#include "S3FP_ParseArgs.h" 

extern "C" {
#include "quadmath.h"
} 


__float128 naive_random128 (__float128 lb, __float128 ub) {
  const long RSCALE = 100000000; 

  __float128 frac = (__float128)(rand() % RSCALE) / (HFP_TYPE) RSCALE;
  __float128 ret = (lb + (ub - lb) * frac); 

  return ret; 
}


__float128 
random128 (__float128 lb, __float128 ub, ENUM_RANDOM_FUNC rfunc) {
  assert(lb <= ub); 

  switch (rfunc) {
  case NAIVE_RANDOM_FUNC:
    return naive_random128(lb, ub);
    break;
  default:
    assert(false && "ERROR: Unknown random number function"); 
    break; 
  }
}
