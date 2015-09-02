#include "S3FP_OutputParser.h"
#include "S3FP_SignatureGen.h" 
#include <vector> 


#ifndef S3FP_DIFCON_MODE 
#define S3FP_DIFCON_MODE
enum ENUM_DIFCON_MODE {
  FROM_OUTPUT_DIFCON_MODE, 
  FROM_SIG_DIFCON_MODE
}; 
#endif // #ifndef S3FP_DIFCON_MODE


bool 
Difcon_SingleIntegerEq (std::vector<HFP_TYPE> data_lp, 
			std::vector<HFP_TYPE> data_hp); 

bool 
Difcon_SingleUnsignedIntegerEq (std::vector<HFP_TYPE> data_lp, 
				std::vector<HFP_TYPE> data_hp); 

bool 
Difcon_LastIntegerEq (std::vector<HFP_TYPE> data_lp, 
		      std::vector<HFP_TYPE> data_hp); 

bool 
Difcon_LastUnsignedIntegerEq (std::vector<HFP_TYPE> data_lp, 
			      std::vector<HFP_TYPE> data_hp); 

bool 
Difcon_IntegerSequenceEq (std::vector<HFP_TYPE> data_lp, 
			  std::vector<HFP_TYPE> data_hp); 

bool 
Difcon_IntegerSequencePrefix (std::vector<HFP_TYPE> data_lp, 
			      std::vector<HFP_TYPE> data_hp); 

bool 
Difcon_BoundedDiffLeastSquareError (std::vector<HFP_TYPE> data_lp, 
				    std::vector<HFP_TYPE> data_hp); 

bool 
Difcon_HighImpliesLow (std::vector<HFP_TYPE> data_lp, 
		       std::vector<HFP_TYPE> data_hp); 
