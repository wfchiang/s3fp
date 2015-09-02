#include "S3FP_OutputParser.h"
#include <vector> 

#include <iostream> 
using namespace std; 

// ==== routines for extracting output values ====
HFP_TYPE  
Outpar_SingleOutputValue (std::vector<HFP_TYPE> poutput) {
  assert(poutput.size() == 1); 
  return poutput[0]; 
}


HFP_TYPE 
Outpar_FirstOutputValue (std::vector<HFP_TYPE> poutput) {
  assert(poutput.size() >= 1); 
  return poutput[0]; 
}


HFP_TYPE 
Outpar_LastOutputValue (std::vector<HFP_TYPE> poutput) {
  assert(poutput.size() >= 1); 
  return poutput[poutput.size()-1]; 
}


void 
Outpar_SubValueStream (std::vector<HFP_TYPE> poutput, 
		       std::vector<HFP_TYPE>& prel) { 
  assert(poutput.size() >= 2); 
  assert(prel.size() == 0);
  HFP_TYPE start_index = poutput[0]; 
  assert(start_index == floorq(poutput[0])); 
  HFP_TYPE vs_length = poutput[1]; 
  assert(vs_length == floorq(poutput[1])); 
  assert(poutput.size() >= (start_index + vs_length));
  for (unsigned int i = 0 ; i < vs_length ; i++) {
    prel.push_back(poutput[start_index + i]); 
  }
}


void 
Outpar_HeadHalfOutputValues (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE>& prel) { 
  assert(poutput.size() % 2 == 0); 

  prel.clear();
  for (unsigned int i = 0 ; i < poutput.size() / 2 ; i++) 
    prel.push_back(poutput[i]); 
}


void 
Outpar_TailHalfOutputValues (std::vector<HFP_TYPE> poutput, std::vector<HFP_TYPE>& prel) {
  assert(poutput.size() % 2 == 0); 

  prel.clear();
  for (unsigned int i = poutput.size() / 2 ; i < poutput.size() ; i++) 
    prel.push_back(poutput[i]); 
}


