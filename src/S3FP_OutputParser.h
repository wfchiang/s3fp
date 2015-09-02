#include "definitions.h"
#include <vector> 

HFP_TYPE  
Outpar_SingleOutputValue (std::vector<HFP_TYPE> poutput); 

HFP_TYPE 
Outpar_FirstOutputValue (std::vector<HFP_TYPE> poutput); 

HFP_TYPE 
Outpar_LastOutputValue (std::vector<HFP_TYPE> poutput); 

void 
Outpar_SubValueStream (std::vector<HFP_TYPE> poutput, 
		       std::vector<HFP_TYPE>& prel); 

void 
Outpar_HeadHalfOutputValues (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> prel); 

void 
Outpar_TailHalfOutputValues (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> prel); 

