#include "S3FP_DifferentialContract.h" 
#include <vector> 


bool 
Difcon_SingleIntegerEq (std::vector<HFP_TYPE> data_lp, 
			std::vector<HFP_TYPE> data_hp) {
  assert(data_lp.size() == 1); 
  assert(data_hp.size() == 1); 

  std::vector<HFP_TYPE> sig_lp; 
  std::vector<HFP_TYPE> sig_hp; 

  Siggen_SingleInteger(data_lp, sig_lp); 
  Siggen_SingleInteger(data_hp, sig_hp); 
  
  assert(sig_lp.size() == 1); 
  assert(sig_hp.size() == 1); 
 
  bool ret = (sig_lp[0] == sig_hp[0]); 

  return ret; 
}


bool 
Difcon_SingleUnsignedIntegerEq (std::vector<HFP_TYPE> data_lp, 
				std::vector<HFP_TYPE> data_hp) {
  assert(data_lp.size() == 1); 
  assert(data_hp.size() == 1); 
  assert(data_lp[0] >= 0); 
  assert(data_hp[0] >= 0); 

  std::vector<HFP_TYPE> sig_lp; 
  std::vector<HFP_TYPE> sig_hp; 

  bool ret = Difcon_SingleIntegerEq(data_lp, data_hp); 
  
  return ret; 
}


bool 
Difcon_LastIntegerEq (std::vector<HFP_TYPE> data_lp, 
		      std::vector<HFP_TYPE> data_hp) {
  std::vector<HFP_TYPE> sig_lp; 
  std::vector<HFP_TYPE> sig_hp; 

  Siggen_LastInteger(data_lp, sig_lp); 
  Siggen_LastInteger(data_hp, sig_hp); 

  bool ret = (sig_lp[0] == sig_hp[0]); 
  
  return ret; 
}


bool 
Difcon_LastUnsignedIntegerEq (std::vector<HFP_TYPE> data_lp, 
			      std::vector<HFP_TYPE> data_hp) {
  assert(data_lp[data_lp.size()-1] >= 0);
  assert(data_hp[data_hp.size()-1] >= 0);

  std::vector<HFP_TYPE> sig_lp; 
  std::vector<HFP_TYPE> sig_hp; 

  bool ret = Difcon_LastIntegerEq(data_lp, data_hp); 
  
  return ret; 
}


bool 
Difcon_IntegerSequenceEq (std::vector<HFP_TYPE> data_lp, 
			  std::vector<HFP_TYPE> data_hp) {
  unsigned int i = 0; 
  std::vector<HFP_TYPE> sig_lp;
  std::vector<HFP_TYPE> sig_hp; 
  
  Siggen_IntegerSequence(data_lp, sig_lp); 
  Siggen_IntegerSequence(data_hp, sig_hp); 

  assert(sig_lp.size() == sig_hp.size()); 
  assert(sig_lp.size() >= 1); 

  bool ret = true; 
  for (unsigned int i = 0 ; i < sig_lp.size() ; i++) {
    if (sig_lp != sig_hp) {
      ret = false; 
      break; 
    }
  }
 
  return ret; 
}


bool 
Difcon_IntegerSequencePrefix (std::vector<HFP_TYPE> data_lp, 
			      std::vector<HFP_TYPE> data_hp) {
  std::vector<HFP_TYPE> is_lp; 
  std::vector<HFP_TYPE> is_hp; 

  for (unsigned int i = 0 ; (i < data_lp.size() && i < data_hp.size()) ; i++) {
    is_lp.push_back(data_lp[i]); 
    is_hp.push_back(data_hp[i]); 
  }

  return Difcon_IntegerSequenceEq(is_lp, is_hp); 
}


bool 
Difcon_BoundedDiffLeastSquareError (std::vector<HFP_TYPE> data_lp, 
				    std::vector<HFP_TYPE> data_hp) {
  assert(data_lp.size() == data_hp.size()); 
  assert(data_lp.size() >= 2); 
  assert(data_lp[0] == data_hp[0]); 

  HFP_TYPE lse_bound = data_lp[0]; 

  std::vector<HFP_TYPE> diff; 
  for (unsigned int i = 1 ; i < data_lp.size() ; i++) 
    diff.push_back(data_lp[i] - data_hp[i]); 

  std::vector<HFP_TYPE> lse_buff; 
  Siggen_LeastSquareError(diff, lse_buff); 
  assert(lse_buff.size() == 1); 
  HFP_TYPE lse = lse_buff[0]; 
  
  bool ret = (lse <= lse_bound); 
  
  return ret; 
}


bool 
Difcon_HighImpliesLow (std::vector<HFP_TYPE> data_lp, 
		       std::vector<HFP_TYPE> data_hp) {
  assert(data_lp.size() == 1);
  assert(data_hp.size() == 1); 

  bool dec_lp = ((data_lp[0] == 0.0) ? false : true); 
  bool dec_hp = ((data_hp[0] == 0.0) ? false : true); 
  
  bool ret = (!(dec_hp) || dec_lp); 

  return ret; 
}

