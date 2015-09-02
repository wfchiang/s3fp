#include "S3FP_ErrorOptimization.h" 
#include <math.h>
#include <vector> 

#include <iostream>

using namespace std; 

HFP_TYPE 
Erropt_ErrorFilter (HFP_TYPE err) {
  if (isinfq(err)) {
    cout << "Warning: +/-inf as error -> convert to zero" << endl;
    err = 0.0; 
  }
  else if (isnanq(err)) {
    cout << "Warning: NaN as error -> convert to zero" << endl;
    err = 0.0; 
  }

  return err; 
}


HFP_TYPE 
Erropt_FirstValue (vector<HFP_TYPE> lp_outputs, 
		  vector<HFP_TYPE> hp_outputs, 
		  ENUM_ERR_FUNC err_func, 
		  HFP_TYPE rel_padding) {
  HFP_TYPE lpv = Outpar_FirstOutputValue(lp_outputs); 
  HFP_TYPE hpv = Outpar_FirstOutputValue(hp_outputs); 

  HFP_TYPE ret = 0.0; 

  switch (err_func) {
  case REL_ERR_FUNC: 
    ret = Errfun_RelativeError(lpv, hpv, 
			       rel_padding); 
    break; 
  case ABSREL_ERR_FUNC: 
    ret = Errfun_AbsRelativeError(lpv, hpv, 
				  rel_padding); 
    break; 
  case ABSRELMAXONE_ERR_FUNC:
    ret = Errfun_AbsRelativeMaxOneError(lpv, hpv, 
					rel_padding); 
    break; 
  case ABS_ERR_FUNC:
    ret = Errfun_AbsoluteError(lpv, hpv); 
    break; 
  case ABSABS_ERR_FUNC:
    ret = Errfun_AbsAbsoluteError(lpv, hpv); 
    break; 
  case LOWP_ERR_FUNC:
    ret = Errfun_LowpValue(lpv, hpv); 
    break; 
  default:
    assert(false && "Error: Invalid Error Function"); 
    break; 
  }

  return Erropt_ErrorFilter(ret); 
}


HFP_TYPE 
Erropt_LastValue (vector<HFP_TYPE> lp_outputs, 
		  vector<HFP_TYPE> hp_outputs, 
		  ENUM_ERR_FUNC err_func, 
		  HFP_TYPE rel_padding) {
  HFP_TYPE lpv = Outpar_LastOutputValue(lp_outputs); 
  HFP_TYPE hpv = Outpar_LastOutputValue(hp_outputs); 

  HFP_TYPE ret = 0.0; 

  switch (err_func) {
  case REL_ERR_FUNC: 
    ret = Errfun_RelativeError(lpv, hpv, 
				rel_padding); 
    break; 
  case ABSREL_ERR_FUNC: 
    ret = Errfun_AbsRelativeError(lpv, hpv, 
				   rel_padding); 
    break; 
  case ABSRELMAXONE_ERR_FUNC:
    ret = Errfun_AbsRelativeMaxOneError(lpv, hpv, 
					rel_padding); 
    break; 
  case ABS_ERR_FUNC:
    ret = Errfun_AbsoluteError(lpv, hpv); 
    break; 
  case ABSABS_ERR_FUNC:
    ret = Errfun_AbsAbsoluteError(lpv, hpv); 
    break; 
  case LOWP_ERR_FUNC:
    ret = Errfun_LowpValue(lpv, hpv); 
    break; 
  default:
    assert(false && "Error: Invalid Error Function"); 
    break; 
  }

  return Erropt_ErrorFilter(ret); 
}


HFP_TYPE 
Erropt_Summation (vector<HFP_TYPE> lp_outputs, 
		  vector<HFP_TYPE> hp_outputs, 
		  ENUM_ERR_FUNC err_func, 
		  HFP_TYPE rel_padding) {
  assert(lp_outputs.size() == hp_outputs.size()); 

  unsigned long i = 0; 
  HFP_TYPE err = 0.0; 

  for (i = 0 ; i < lp_outputs.size() ; i++) {
    switch (err_func) {
    case REL_ERR_FUNC: 
      err += Errfun_RelativeError(lp_outputs[i], 
				  hp_outputs[i], 
				  rel_padding); 
      break; 
    case ABSREL_ERR_FUNC: 
      err += Errfun_AbsRelativeError(lp_outputs[i], 
				     hp_outputs[i], 
				     rel_padding); 
      break; 
    case ABSRELMAXONE_ERR_FUNC: 
      err += Errfun_AbsRelativeMaxOneError(lp_outputs[i], 
					   hp_outputs[i], 
					   rel_padding); 
      break; 
    case ABS_ERR_FUNC:
      err += Errfun_AbsoluteError(lp_outputs[i], 
				  hp_outputs[i]); 
      break; 
    case ABSABS_ERR_FUNC:
      err += Errfun_AbsAbsoluteError(lp_outputs[i], 
				     hp_outputs[i]); 
      break; 
    case LOWP_ERR_FUNC:
      err += Errfun_LowpValue(lp_outputs[i], 
			      hp_outputs[i]); 
      break; 
    default:
      assert(false && "Error: Invalid Error Function"); 
      break; 
    }
  }

  return Erropt_ErrorFilter(err); 
}


HFP_TYPE 
Erropt_Average (vector<HFP_TYPE> lp_outputs, 
		vector<HFP_TYPE> hp_outputs, 
		ENUM_ERR_FUNC err_func, 
		HFP_TYPE rel_padding) {
  assert(lp_outputs.size() == hp_outputs.size()); 
  assert(lp_outputs.size() > 0); 

  HFP_TYPE errsum = Erropt_Summation(lp_outputs, 
				     hp_outputs, 
				     err_func, 
				     rel_padding); 
  return Erropt_ErrorFilter(errsum / (HFP_TYPE) lp_outputs.size()); 
}


HFP_TYPE 
Erropt_Maximum (vector<HFP_TYPE> lp_outputs, 
		vector<HFP_TYPE> hp_outputs, 
		ENUM_ERR_FUNC err_func, 
		HFP_TYPE rel_padding) {
  assert(lp_outputs.size() == hp_outputs.size()); 

  unsigned long i = 0; 
  HFP_TYPE maxerr = 0.0; 
  HFP_TYPE thiserr = 0.0; 

  for (i = 0 ; i < lp_outputs.size() ; i++) {
    switch (err_func) {
    case REL_ERR_FUNC: 
      assert(false && "Error: Invalid Error Function for MAX_ERR_OPT"); 
      break; 
    case ABSREL_ERR_FUNC: 
      thiserr = Errfun_AbsRelativeError(lp_outputs[i], 
					hp_outputs[i], 
					rel_padding); 
      break; 
    case ABSRELMAXONE_ERR_FUNC: 
      thiserr = Errfun_AbsRelativeMaxOneError(lp_outputs[i], 
					      hp_outputs[i], 
					      rel_padding); 
      break; 
    case ABS_ERR_FUNC:
      assert(false && "Error: Invalid Error Function for MAX_ERR_OPT"); 
      break; 
    case ABSABS_ERR_FUNC:
      thiserr = Errfun_AbsAbsoluteError(lp_outputs[i], 
					hp_outputs[i]); 
      break; 
    case LOWP_ERR_FUNC:
      assert(false && "Error: Invalid Error Function for MAX_ERR_OPT"); 
      break; 
    default:
      assert(false && "Error: Invalid Error Function"); 
      break; 
    }

    if (thiserr > maxerr) {
      maxerr = thiserr; 
    }
  }

  return Erropt_ErrorFilter(maxerr); 
}


void 
Erropt_CalculateError (vector<HFP_TYPE> lp_outputs, 
		       vector<HFP_TYPE> hp_outputs, 
		       ENUM_ERR_OPT err_opt, 
		       ENUM_ERR_FUNC err_func, 
		       HFP_TYPE rel_padding, 
		       vector<HFP_TYPE> &errs) {
  assert(errs.size() == 0); 

  switch (err_opt) {
  case FIRST_ERR_OPT:
    errs.push_back(Erropt_FirstValue(lp_outputs, 
				     hp_outputs, 
				     err_func, 
				     rel_padding)); 
    break; 
  case LAST_ERR_OPT:
    errs.push_back(Erropt_LastValue(lp_outputs, 
				    hp_outputs, 
				    err_func, 
				    rel_padding));
    break; 
  case SUM_ERR_OPT:
    errs.push_back(Erropt_Summation(lp_outputs, 
				    hp_outputs, 
				    err_func, 
				    rel_padding)); 
    break; 
  case AVE_ERR_OPT: 
    errs.push_back(Erropt_Average(lp_outputs, 
				  hp_outputs, 
				  err_func, 
				  rel_padding)); 
    break; 
  case MAX_ERR_OPT:
    errs.push_back(Erropt_Maximum(lp_outputs, 
				  hp_outputs, 
				  err_func, 
				  rel_padding)); 
    break; 
  default: 
    assert(false && "Error: Invalid Error Optimization"); 
    break; 
  }
} 
