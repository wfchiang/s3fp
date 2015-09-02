#include "S3FP_OutputParser.h"
#include "S3FP_SignatureGen.h"
#include <vector>  


void 
Siggen_SingleValue (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE>& psig) {
  HFP_TYPE sig = Outpar_SingleOutputValue(poutput); 
  
  psig.clear(); 
  psig.push_back(sig); 
}


void 
Siggen_SingleInteger (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE>& psig) {
  Siggen_SingleValue(poutput, psig); 
  
  assert(psig[0] == floorq(psig[0])); 
}


void 
Siggen_SingleUnsignedInteger (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> & psig) {
  Siggen_SingleInteger(poutput, psig); 

  assert(psig[0] >= 0.0); 
}


void 
Siggen_LastValue (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> & psig) {
  HFP_TYPE sig = Outpar_LastOutputValue(poutput); 

  psig.clear(); 
  psig.push_back(sig); 
}


void 
Siggen_LastInteger (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> & psig) {
  Siggen_LastValue(poutput, psig); 

  assert(psig[0] == floorq(psig[0])); 
}


void 
Siggen_LastUnsignedInteger (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> & psig) {
  Siggen_LastInteger(poutput, psig); 

  assert(psig[0] >= 0.0); 
}


void 
Siggen_IntegerSequence (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE> &psig) {
  assert(poutput.size() >= 1); 

  for (unsigned int i = 0 ; i < poutput.size() ; i++) 
    assert(poutput[i] == floorq(poutput[i])); 

  psig.clear(); 
  psig = poutput; 
}


void 
Siggen_LeastSquareError (std::vector<HFP_TYPE> poutput, 
			 std::vector<HFP_TYPE> &psig) {
  assert(poutput.size() >= 1); 

  HFP_TYPE lse = 0.0; 

  for (unsigned int i = 0 ; i < poutput.size() ; i++) 
    lse += (poutput[i] * poutput[i]); 

  psig.clear(); 
  psig.push_back(lse); 
}
