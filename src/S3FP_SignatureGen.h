#include "definitions.h"
#include <vector> 

void 
Siggen_SingleValue (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> & psig); 

void 
Siggen_SingleInteger (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> & psig); 

void 
Siggen_SingleUnsignedInteger (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> & psig); 

void 
Siggen_LastValue (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> & psig); 

void 
Siggen_LastInteger (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> & psig); 

void 
Siggen_LastUnsignedInteger (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> & psig); 

void 
Siggen_IntegerSequence (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> &psig); 

void 
Siggen_LeastSquareError (std::vector<HFP_TYPE> poutput, 
			 std::vector<HFP_TYPE> &psig); 
