#include "definitions.h" 
#include "S3FP_ParseArgs.h"
#include "S3FP_OutputParser.h"
#include "S3FP_ErrorFunction.h" 
#include <vector> 

HFP_TYPE 
Erropt_ErrorFilter (HFP_TYPE err); 

HFP_TYPE 
Erropt_FirstValue (std::vector<HFP_TYPE> lp_outputs, 
		   std::vector<HFP_TYPE> hp_outputs, 
		   ENUM_ERR_FUNC err_func, 
		   HFP_TYPE rel_padding); 

HFP_TYPE 
Erropt_LastValue (std::vector<HFP_TYPE> lp_outputs, 
		  std::vector<HFP_TYPE> hp_outputs, 
		  ENUM_ERR_FUNC err_func, 
		  HFP_TYPE rel_padding); 

HFP_TYPE 
Erropt_Summation (std::vector<HFP_TYPE> lp_outputs, 
		  std::vector<HFP_TYPE> hp_outputs, 
		  ENUM_ERR_FUNC err_func, 
		  HFP_TYPE rel_padding); 

HFP_TYPE 
Erropt_SubSummation (vector<HFP_TYPE> lp_outputs, 
		     vector<HFP_TYPE> hp_outputs, 
		     ENUM_ERR_FUNC err_func, 
		     HFP_TYPE rel_padding); 

HFP_TYPE 
Erropt_Average (vector<HFP_TYPE> lp_outputs, 
		vector<HFP_TYPE> hp_outputs, 
		ENUM_ERR_FUNC err_func, 
		HFP_TYPE rel_padding); 

HFP_TYPE 
Erropt_Maximum (std::vector<HFP_TYPE> lp_outputs, 
		std::vector<HFP_TYPE> hp_outputs, 
		ENUM_ERR_FUNC err_func, 
		HFP_TYPE rel_padding); 

void 
Erropt_CalculateError (vector<HFP_TYPE> lp_outputs, 
		       vector<HFP_TYPE> hp_outputs, 
		       ENUM_ERR_OPT err_opt, 
		       ENUM_ERR_FUNC err_func, 
		       HFP_TYPE rel_padding, 
		       vector<HFP_TYPE> &errs); 
