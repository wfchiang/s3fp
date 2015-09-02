#include "definitions.h" 

HFP_TYPE 
Errfun_RelativeError (HFP_TYPE approx, 
		      HFP_TYPE exact, 
		      HFP_TYPE rel_padding); 

HFP_TYPE 
Errfun_AbsRelativeError (HFP_TYPE approx, 
			 HFP_TYPE exact, 
			 HFP_TYPE rel_padding); 

HFP_TYPE 
Errfun_AbsRelativeMaxOneError (HFP_TYPE approx, 
			       HFP_TYPE exact, 
			       HFP_TYPE rel_padding); 

HFP_TYPE 
Errfun_AbsoluteError (HFP_TYPE approx, 
		      HFP_TYPE exact); 

HFP_TYPE 
Errfun_AbsAbsoluteError (HFP_TYPE approx, 
			 HFP_TYPE exact); 

HFP_TYPE 
Errfun_LowpValue (HFP_TYPE approx, 
		  HFP_TYPE exact); 


