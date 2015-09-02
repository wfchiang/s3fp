#include "S3FP_ErrorFunction.h" 
#include <math.h>


HFP_TYPE 
Errfun_RelativeError (HFP_TYPE approx, 
		      HFP_TYPE exact, 
		      HFP_TYPE rel_padding) {
  assert(rel_padding >= 0); 

  HFP_TYPE diff = exact - approx; 
  HFP_TYPE abs_exact = ((exact >= 0) ? exact : (-1 * exact)); 
  
  abs_exact = ((abs_exact >= rel_padding) ? abs_exact : rel_padding); 
  
  return (diff / abs_exact); 
}


HFP_TYPE 
Errfun_AbsRelativeError (HFP_TYPE approx, 
			 HFP_TYPE exact, 
			 HFP_TYPE rel_padding) {
  HFP_TYPE err = Errfun_RelativeError(approx, exact, rel_padding); 
  err = ((err >= 0) ? err : (-1 * err)); 
  return err; 
}


HFP_TYPE 
Errfun_AbsRelativeMaxOneError (HFP_TYPE approx, 
			       HFP_TYPE exact, 
			       HFP_TYPE rel_padding) {
  HFP_TYPE err = Errfun_AbsRelativeError(approx, exact, rel_padding);
  if (err > 1.0) err = 1.0; 
  return err; 
}


HFP_TYPE 
Errfun_AbsoluteError (HFP_TYPE approx, 
		      HFP_TYPE exact) {
  return approx - exact; 
}


HFP_TYPE 
Errfun_AbsAbsoluteError (HFP_TYPE approx, 
			 HFP_TYPE exact) {
  HFP_TYPE err = Errfun_AbsoluteError(approx, exact); 
  err = ((err >= 0) ? err : (-1 * err)); 
  return err; 
}


HFP_TYPE 
Errfun_LowpValue (HFP_TYPE approx, 
		  HFP_TYPE exact) {
  return approx; 
}
