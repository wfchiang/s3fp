
#include "S3FP_Core.h"
#include "S3FP_OutputParser.h" 
#include "S3FP_SignatureGen.h" 
#include "S3FP_DifferentialContract.h" 
#include "S3FP_ErrorOptimization.h" 


/* 
   function prototypes
*/
pair<HFP_TYPE, HFP_TYPE> 
varRange (unsigned int vindex, unsigned int n_vars, ifstream &irfile);
void randCONF (CONF &ret_conf, unsigned int n_vars, bool plain=false);
void neighborCONFs (vector<CONF> &ret_confs, CONF this_conf, bool just_rand_one=false);
string CONFtoString (CONF conf); 
void dumpCONFtoStdout (CONF conf); 
void dumpCONFtoErrors (CONF conf, vector<FPR> rels);
RTReport grtEvaluateCONF (EvaluationBasis eva_basis, unsigned int N, CONF conf); 
bool TerminationCriterion(time_t ts); 
void IterativeFirstImprovement (CONF conf, CONF &ret_conf, EvaluationBasis eva_basis, long nn_to_explore); 
void complementCONF (CONF base, CONF dsub, CONF &dret); 
void binarySubCONF (CONF base, CONF &dret, char select); 
void generateBinarySubCONFs (CONF base, vector<CONF> &ret_confs, unsigned int n_rand); 
void BinaryPartitionImprovement (CONF conf, CONF &ret_conf, EvaluationBasis eva_basis, unsigned int n_rand, bool unguided=false); 
bool isLittleEndian(); 
template <class FT> 
bool isValidFP (FT in_fp); 
void rtPlainRT (EvaluationBasis eva_basis, unsigned int n_vars ); 
void rtBGRT (EvaluationBasis eva_basis, unsigned int n_vars ); 
void rtILS (EvaluationBasis eva_basis, unsigned int n_vars ); 
void rtPSO (EvaluationBasis eva_basis, unsigned int n_particles, unsigned int n_vars, unsigned int N); 
void rtBURT (EvaluationBasis eva_basis, unsigned int n_vars );
void rtOpt4j (EvaluationBasis eva_basis, unsigned int n_vars );  
void rtErrorMagnificationTest (EvaluationBasis eva_basis, unsigned int n_vars); 
void uniformRobustnessCheck (EvaluationBasis eva_basis, unsigned int n_vars); 
bool analogWhiteBoxSampling (EvaluationBasis eva_basis, unsigned int n_vars); 
void rtAnalogWhiteBoxSampling (EvaluationBasis eva_basis, unsigned int n_vars); 
 

/* 
   main 
*/
int main (int argc, char *argv[]) {
  unsigned int i;

  cout << "==== S3FP arguments and settings ====" << endl;

  assert(S3FP_ParseArgs(N_VARS, 
			INPUT_FILE_NAME, 
			RT_MODE, 
			TIMEOUT, 
			T_RESOURCE, 
			EXE_LP_NAME, 
			OUTPUT_LP_NAME, 
			EXE_HP_NAME, 
			OUTPUT_HP_NAME, 
			RSEED, 
			ERR_OPT, 
			ERR_FUNC, 
			SIG_FUNC, 
			DIFF_CON, 
			OPT_REPRESENT, 
			OPT_RANGE, 
			REL_DELTA,
			UNIFORM_INPUT, 
			UNIFORM_INPUTLB, 
			UNIFORM_INPUTUB, 
			INPUT_RANGE_FILE, 
			RANDOM_FUNC, 
			CHECK_UNSTABLE_ERROR, 
			AWBS_FIXED_INITIALA, 
			AWBS_FIVESTAGE_ASSIST, 
			UNSTABLE_ERROR_REPORT, 
			N_INPUT_REPEATS, 
			backdoor_use_external_input)); 
  
  cout << "input bit-width: " << sizeof(INPUTV_TYPE) << " bytes" << endl;
  cout << "n inputs: " << N_VARS << endl;
  cout << "input_name: " << INPUT_FILE_NAME << endl;
  cout << "LP EXE: " << EXE_LP_NAME << " -> " << OUTPUT_LP_NAME << endl;
  cout << "HP EXE: " << EXE_HP_NAME << " -> " << OUTPUT_HP_NAME << endl;
  cout << "RT_MODE: " << RT_MODE << endl;
  cout << "TIMEOUT: " << TIMEOUT << endl;
  cout << "T_RESOURCE: " << T_RESOURCE << endl;
  cout << "RSEED: " << RSEED << endl;
  cout << "ERR_OPT: " << ERR_OPT << endl;
  cout << "ERR_FUNC: " << ERR_FUNC << endl;
  cout << "OPT_REPRESENT: " << getOPTREPRESENTString(OPT_REPRESENT) << endl;
  cout << "OPT_RANGE: " << getOPTRANGEString(OPT_RANGE) << endl;
  cout << "UNIFORM_INPUT: " << UNIFORM_INPUT << endl;
  cout << "REL_DELTA: " << (CONSOLE_OUTPUT_TYPE) REL_DELTA << endl;
  if (UNIFORM_INPUT) {
    cout << "UNIFORM_INPUTLB: " << (CONSOLE_OUTPUT_TYPE) UNIFORM_INPUTLB << endl;
    cout << "UNIFORM_INPUTUB: " << (CONSOLE_OUTPUT_TYPE) UNIFORM_INPUTUB << endl;	
  }
  if (UNIFORM_INPUT == false)
    cout << "INPUT_RANGE_FILE: " << INPUT_RANGE_FILE << endl;
  cout << "WIDDEN FRAC.: " << (CONSOLE_OUTPUT_TYPE) WIDDEN_FRAC << endl;
  cout << "CHECK UNSTABLE ERROR: " << CHECK_UNSTABLE_ERROR << endl;
  if (CHECK_UNSTABLE_ERROR) {
    cout << "UNSTABLE_ERROR_REPORT: " << UNSTABLE_ERROR_REPORT << endl;
  }
  if (N_INPUT_REPEATS > 1) 
    cout << "N_INPUT_REPEATS: " << N_INPUT_REPEATS << endl;

  cout << "=====================================" << endl;

  // ---- initialize evaluation basis ----
  EvaluationBasis eva_basis (N_VARS, 
			     EXE_LP_NAME, OUTPUT_LP_NAME, 
			     EXE_HP_NAME, OUTPUT_HP_NAME, 
			     INPUT_FILE_NAME, 
			     N_INPUT_REPEATS);

  // ---- open unstable error report (file) ----
  if (CHECK_UNSTABLE_ERROR) {
    unstable_report = fopen(UNSTABLE_ERROR_REPORT.c_str(), "w");
    assert(unstable_report != NULL); 
  }

  // -- initialize some more global variables --
  srand(RSEED);
  startTimer(&TSTART);
  HALT_NOW = false;

  switch(RT_MODE) {
  case URT_RT_MODE: // unguided RT
    rtPlainRT (eva_basis, N_VARS);
    break;
  case BGRT_RT_MODE: // binary guided RT
    rtBGRT (eva_basis, N_VARS);
    break;
  case ILS_RT_MODE: // iterated local search 
    // ILSifi = -1;
    // ILSifi = n_vars; 
    // ILSifi = n_vars / 2;
    ILSifi = 20;
    // ILSr = n_vars;
    ILSr = 20;
    ILSs = 20;
    rtILS (eva_basis, N_VARS);
    break;
  case PSO_RT_MODE: // particle swarm 
    rtPSO (eva_basis, SWARM_SIZE, N_VARS, N_SEARCH_ITERATIONS);
    break;
  case BURT_RT_MODE: // binary unguided 
    rtBURT (eva_basis, N_VARS);
    break; 
  case OPT4J_MODE: // opt4j 
    rtOpt4j (eva_basis, N_VARS); 
    break; 
  case UROBUST_CHECK_RT_MODE: // uniform robustness check 
    uniformRobustnessCheck (eva_basis, N_VARS); 
    break; 
  case AWBS_RT_MODE: 
    rtAnalogWhiteBoxSampling (eva_basis, N_VARS); 
    break; 
  default:
    assert(false && "ERROR: not supported RT mode\n");
    break;
  }

  // ---- close unstalbe errro report 
  if (CHECK_UNSTABLE_ERROR)
    fclose(unstable_report); 

  return 0;
}


bool isTimeout(time_t ts, float frac=1) { 
  assert(frac <= 1); 
  time_t tend; 
  time(&tend);
  if (T_RESOURCE == SVE_TRESOURCE) 
    return (N_RTS >= (TIMEOUT*frac));
  else {
    return ((tend - ts) >= (TIMEOUT*frac));
  }
}


// return  1 if r1 "worse" r2 WITH consider our opt. target
// return  0 if r1 == r2                                              
// return -1 if r1 "better" r2 WITH consider our opt. target
// NOTE: r1 and r2 must be valid ranges 
int FPRCompare (FPR &r1, FPR &r2) {
  if ((r1.lb > r1.ub) && (r2.lb > r2.ub)) return 0;
  else if (r1.lb > r1.ub) return 1;
  else if (r2.lb > r2.ub) return -1;
  
  HFP_TYPE r1_error = 0;
  HFP_TYPE r2_error = 0;
  int ret = 0;

  HFP_TYPE abs_r1_ub = (r1.ub>=0)?r1.ub:((-1)*r1.ub);
  HFP_TYPE abs_r1_lb = (r1.lb>=0)?r1.lb:((-1)*r1.lb);
  HFP_TYPE abs_r2_ub = (r2.ub>=0)?r2.ub:((-1)*r2.ub);
  HFP_TYPE abs_r2_lb = (r2.lb>=0)?r2.lb:((-1)*r2.lb);

  switch (OPT_REPRESENT) { 
  case UB_OPT_REPRESENT:
    r1_error = r1.ub;
    r2_error = r2.ub;
    if (r1_error > r2_error) ret = -1;
    else if (r1_error < r2_error) ret = 1; 
    else ret = 0; 
    break;
  case LB_OPT_REPRESENT:
    r1_error = r1.lb;
    r2_error = r2.lb;
    if (r1_error > r2_error) ret = 1; 
    else if (r1_error < r2_error) ret = -1; 
    else ret = 0;
    break;
  case MIX_OPT_REPRESENT:
    r1_error = ((abs_r1_ub>=abs_r1_lb) ? abs_r1_ub : abs_r1_lb);
    r2_error = ((abs_r2_ub>=abs_r2_lb) ? abs_r2_ub : abs_r2_lb);
    if (r1_error > r2_error) ret = -1;
    else if (r1_error < r2_error) ret = 1;
    else ret = 0;
    break;
  default:
    assert(false);
    break;
  }

  if (OPT_RANGE == WIDE_OPT_RANGE) ; 
  else if (OPT_RANGE == NARROW_OPT_RANGE) 
    ret = (-1) * ret; 
  else assert(false && "[ERROR](FPRCompare): No such optimization range");

  return ret;
}

// return  1 if r1 "worse" r2 with considering our opt. target 
// return  0 if r1 == r2                                              
// return -1 if r1 "better" r2 with considering our opt. target
int RTReportCompare(RTReport &r1, RTReport &r2) {
  if (r1.has_best_error == false &&
      r2.has_best_error == false) return 0;
  else if (r1.has_best_error == false &&
           r2.has_best_error == true) return 1;
  else if (r1.has_best_error == true &&
           r2.has_best_error == false) return -1;
  else {    
    return FPRCompare(r1.best_error, r2.best_error);
  }

  assert(false);
}


bool TerminationCriterion(time_t ts) { 
  if (CHECK_UNSTABLE_ERROR && HALT_NOW) return true; 
  return isTimeout(ts); 
}


pair<HFP_TYPE, HFP_TYPE> 
varRange (unsigned int vindex, unsigned int n_vars, ifstream &irfile) {
  if (UNIFORM_INPUT) 
    return pair<HFP_TYPE, HFP_TYPE>(UNIFORM_INPUTLB, UNIFORM_INPUTUB);
  else {
    long double thislb;
    long double thisub;
    assert(irfile.is_open());
    assert(irfile.eof() == false);
    irfile >> thislb;
    assert(irfile.is_open());
    assert(irfile.eof() == false);
    irfile >> thisub;
    
    return pair<HFP_TYPE, HFP_TYPE>((HFP_TYPE)thislb, (HFP_TYPE)thisub);
  }
}


void randCONF (CONF &ret_conf, unsigned int n_vars, bool plain) {
  ifstream irfile; 
  if (UNIFORM_INPUT == false) {
    irfile.open(INPUT_RANGE_FILE.c_str(), ifstream::in);
    assert(irfile.is_open());
  }
  

  if (ret_conf.size() != 0) ret_conf.clear();
  for (unsigned int vi = 0 ; vi < n_vars ; vi++) {
    pair<HFP_TYPE, HFP_TYPE> var_range = varRange(vi, n_vars, irfile);

    if (plain == false) {
      FPR fpr (var_range.first, var_range.second, 
	       rand()%RANGE_GRANULARITY, RANGE_GRANULARITY);
      ret_conf.push_back(pair<unsigned int, FPR>(vi, fpr));
    }
    else {
      FPR fpr (var_range.first, var_range.second, 
	       0, 1);
      ret_conf.push_back(pair<unsigned int, FPR>(vi, fpr));
    }
  }

  if (UNIFORM_INPUT == false)
    irfile.close();
}


// when n_rands == -1 --> enumerate all possible neighbors 
void neighborCONFs (vector<CONF> &ret_confs, CONF this_conf, bool just_rand_one) {
  assert(just_rand_one || 
	 (just_rand_one == false && ret_confs.size() == 0));
  unsigned int n_vars = this_conf.size();

  unsigned int vi = 0; 
  bool up_neighbor = true;
  while (vi < n_vars) {
    if (just_rand_one) {
      vi = rand() % n_vars;
      if (rand() % 2 == 0)
	up_neighbor = true;
      else 
	up_neighbor = false;
    }

    FPR this_fpr = this_conf[vi].second;

    if (((just_rand_one && up_neighbor) || (just_rand_one == false)) && 
	(this_fpr.index_pars + 1 < this_fpr.n_pars)) {
      CONF new_conf = this_conf;
      (new_conf[vi].second.index_pars)++;
      ret_confs.push_back(new_conf);
      if (just_rand_one) break;
    }
    if (((just_rand_one && up_neighbor == false) || (just_rand_one == false)) && 
	(this_fpr.index_pars > 0)) {
      CONF new_conf = this_conf;
      (new_conf[vi].second.index_pars)--;
      ret_confs.push_back(new_conf);
      if (just_rand_one) break;
    } 

    if (just_rand_one == false) 
      vi++;
  }
}


/**
 * Computes the ulp error of two floating-point numbers.
 * The parameter p specifies the intended precision of floating-point numbers:
 * p = 24 for float
 * p = 53 for double
 * p = FLT128_MANT_DIG for __float128
 */
__float128 ulp_error(__float128 approx, __float128 exact, int p)
{
  if (!isValidFP(approx) || !isValidFP(exact)) {
    return 0;
  }

  int approxExp;
  __float128 approxSignificand = frexpq(approx, &approxExp);
  __float128 diff = approxSignificand - ldexpq(exact, -approxExp);

  __float128 ulp = ldexpq(diff, p);
  /*
  cout << "[ulp_error]: " << (long double) approx << " vs " << (long double) exact << endl;
  cout << "    " << (long double) approxSignificand << " : " << (long double) diff << " : " << (long double) ulp << endl; 
  */
  return ulp;
}

__float128 ulp_error_2(__float128 approx, __float128 exact, int p)
{
  if (!isValidFP(approx) || !isValidFP(exact)) {
    return 0;
  }

  int approxExp;
  int exactExp;
  __float128 approxSignificand = frexpq(approx, &approxExp);
  __float128 exactSignificand = frexpq(exact, &exactExp);
  __float128 diff = approxSignificand - ldexpq(exactSignificand, (exactExp-approxExp));
  __float128 ulp = ldexpq(diff, p);
  /*
  cout << "[ulp_error]: " << (long double) approx << " vs " << (long double) exact << endl;
  cout << "    " << (long double) approxSignificand << " : " << (long double) exactSignificand << endl;
  cout << "    " << (long double) approxExp << " : " << (long double) exactExp << endl;
  cout << "    " << (long double) diff << " : " << (long double) ulp << endl; 
  cout << "----" << (long double) (ldexpq(approxSignificand, p) - ldexpq(exactSignificand, p)) << endl;
  */
  return ulp;
}


bool 
IsDifferentialContractViolated (vector<HFP_TYPE> data_lp, 
				vector<HFP_TYPE> data_hp, 
				ENUM_SIG_FUNC sig_func, 
				const char *input_filename) {
  bool sig_comp = false; 

  vector<HFP_TYPE> sig_lp; 
  vector<HFP_TYPE> sig_hp; 

  switch (sig_func) {
  case SINGLE_INT_SIG_FUNC: 
    sig_comp = Difcon_SingleIntegerEq (data_lp, data_hp); 
    break; 
  case SINGLE_UINT_SIG_FUNC:
    sig_comp = Difcon_SingleUnsignedIntegerEq (data_lp, data_hp); 
    break; 
  case LAST_INT_SIG_FUNC: 
    sig_comp = Difcon_LastIntegerEq (data_lp, data_hp); 
    break; 
  case LAST_UINT_SIG_FUNC: 
    sig_comp = Difcon_LastUnsignedIntegerEq (data_lp, data_hp); 
    break;
  case BOUNDED_LSE_SIG_FUNC: 
    assert(data_lp.size() >= 2);
    assert(data_hp.size() >= 2);
    data_lp[0] = 0; 
    data_hp[0] = 0; 
    sig_comp = Difcon_BoundedDiffLeastSquareError (data_lp, data_hp); 
    break; 
  case HIGH_IMPLIES_LOW_SIG_FUNC: 
    assert(data_lp.size() == 1); 
    assert(data_hp.size() == 1); 
    sig_comp = Difcon_HighImpliesLow (data_lp, data_hp); 
    break; 
  case FIVESTAGE_SIG_FUNC:
    assert(data_lp.size() == 1);
    assert(data_hp.size() == 1); 
    assert(data_lp[0] == 0.0 || 
	   data_lp[0] == 1.0 || 
	   data_lp[0] == 2.0 || 
	   data_lp[0] == 3.0 || 
	   data_lp[0] == 4.0); 
    assert(data_hp[0] == 0.0 || 
	   data_hp[0] == 1.0 || 
	   data_hp[0] == 2.0 || 
	   data_hp[0] == 3.0 || 
	   data_hp[0] == 4.0); 
    sig_comp = (data_lp[0] == data_hp[0]); 
    break; 
  default: 
    assert(false && "ERROR: No such SIG_FUNC"); 
  }
  
  return (!sig_comp); 
}


bool 
IsDivergenceTriggered (vector<HFP_TYPE> data_lp, 
		       vector<HFP_TYPE> data_hp, 
		       ENUM_SIG_FUNC diff_con, 
		       const char *input_filename) { 
  bool is_div = false; 

  switch (diff_con) {
  case SINGLE_INT_SIG_FUNC: 
  case SINGLE_UINT_SIG_FUNC:
  case LAST_INT_SIG_FUNC: 
  case LAST_UINT_SIG_FUNC:
  case HIGH_IMPLIES_LOW_SIG_FUNC: 
    is_div = IsDifferentialContractViolated (data_lp, 
					     data_hp, 
					     diff_con, 
					     input_filename); 
    break; 
  case BOUNDED_LSE_SIG_FUNC: 
    is_div = (!Difcon_BoundedDiffLeastSquareError (data_lp, data_hp)); 
    break; 
  case FIVESTAGE_SIG_FUNC: 
    assert(data_lp.size() == 1);
    assert(data_hp.size() == 1); 
    assert(data_lp[0] == 0.0 || 
	   data_lp[0] == 1.0 || 
	   data_lp[0] == 2.0 || 
	   data_lp[0] == 3.0 || 
	   data_lp[0] == 4.0); 
    assert(data_hp[0] == 0.0 || 
	   data_hp[0] == 1.0 || 
	   data_hp[0] == 2.0 || 
	   data_hp[0] == 3.0 || 
	   data_hp[0] == 4.0); 
    is_div = (data_lp[0] == 0.0 & 
	      (data_hp[0] == 1.0 || data_hp[0] == 2.0 || data_hp[0] == 3.0)); 
    break; 
  default: 
    assert(false && "Error: Unhandled DIFF_CON"); 
  } 
  
  return is_div; 
}


/*
  Functions for Running executables and calculating errors
*/
// return bool to indicate whether the global best configuration need to be updated 
bool 
UpdateRTReport (EvaluationBasis eva_basis, 
		vector<HFP_TYPE> vLPs, vector<HFP_TYPE> vHPs, 
		const char *input_filename, 
		HFP_TYPE &ret_fperr) { 
  vector<HFP_TYPE> fperrs; 
  unsigned int n_outputs = vLPs.size();

  if (CHECK_UNSTABLE_ERROR && HALT_NOW) return false;

  vector<HFP_TYPE> lp_err_est_vals; 
  vector<HFP_TYPE> hp_err_est_vals; 
  vector<HFP_TYPE> lp_div_det_vals;
  vector<HFP_TYPE> hp_div_det_vals; 
  ExtractEEstSigValues(vLPs, lp_err_est_vals, lp_div_det_vals); 
  ExtractEEstSigValues(vHPs, hp_err_est_vals, hp_div_det_vals); 

  Erropt_CalculateError (lp_err_est_vals, hp_err_est_vals, 
			 ERR_OPT, 
			 ERR_FUNC, 
			 REL_DELTA, 
			 fperrs); 

  assert(fperrs.size() == 1); 
  ret_fperr = fperrs[0]; 
  
#ifdef S3FP_VERBOSE 
  cout << "LAST error = " << (CONSOLE_OUTPUT_TYPE) ret_fperr << endl; 
#endif 

  // If we decide to check unstable error, 
  // decide whether to halt now or not 
  if (CHECK_UNSTABLE_ERROR) {
    bool to_halt = IsDivergenceTriggered(lp_div_det_vals, 
					 hp_div_det_vals, 
					 DIFF_CON, 
					 input_filename); 
    if (to_halt) HALT_NOW = true; 
  }

  bool is_valid_fp = false;
  is_valid_fp = isValidFP<HFP_TYPE>(ret_fperr);

  if (is_valid_fp) {    
    // cout << "vLP: " << (long double)vLP << "  vs  " << "vHP: " << (long double)vHP << endl;
    N_VALID_RTS++; 
    local_best.addError(ret_fperr, input_filename); 
    bool beat_global_best_conf = GLOBAL_BEST.addError(ret_fperr, input_filename); 
    if (beat_global_best_conf) {
      cout << "Update Global: " << (CONSOLE_OUTPUT_TYPE) ret_fperr << endl;
    }

    // calculate statistical data
    // ERROR_SUM += fperr; 
    ERROR_SUM += (ret_fperr >= 0 ? ret_fperr : ((-1)*ret_fperr)); 
    ERROR_SUM_2 += (ret_fperr * ret_fperr);

    return beat_global_best_conf;      
  }

  return false;
}


// return the local best result
// WARNING: when N == 0 -> infinite number of tests!!
RTReport 
grtEvaluateCONF (EvaluationBasis eva_basis, unsigned int N, CONF conf) {
  // query the evaluation cache first 

  local_best.reset();

  assert(N % eva_basis.getNInputRepeats() == 0);
  for (unsigned int n = 0 ; 
       (n < N || N==0) && (HALT_NOW == false) ; 
       n = n + eva_basis.getNInputRepeats()) {
    // generate input file 
    if (backdoor_use_external_input) ; 
    else
      eva_basis.prepareInput(conf, 
			     RANDOM_FUNC, 
			     false); 

    vector<HFP_TYPE> vLPs;
    vector<HFP_TYPE> vHPs;
    int lpErr, hpErr;

    // run the EXEs
    eva_basis.runLP(&lpErr, vLPs);
    eva_basis.runHP(&hpErr, vHPs);
    assert(vLPs.size() % eva_basis.getNInputRepeats() == 0);
    assert(vHPs.size() % eva_basis.getNInputRepeats() == 0);
    
    long int n_outputs_per_input_vLP = vLPs.size() / eva_basis.getNInputRepeats();
    long int n_outputs_per_input_vHP = vHPs.size() / eva_basis.getNInputRepeats(); 

    // read the output files 
    vector<HFP_TYPE> this_vLPs; 
    vector<HFP_TYPE> this_vHPs;
    for (unsigned int oi = 0 ; 
	 (oi < eva_basis.getNInputRepeats()) && (HALT_NOW == false) ; 
	 oi++) {
      this_vLPs.clear();
      this_vHPs.clear();
      for (unsigned int jj = 0 ; jj < n_outputs_per_input_vLP ; jj++) 
	this_vLPs.push_back(vLPs[oi*n_outputs_per_input_vLP + jj]);
      for (unsigned int jj = 0 ; jj < n_outputs_per_input_vHP ; jj++) 
	this_vHPs.push_back(vHPs[oi*n_outputs_per_input_vHP + jj]);
      
      HFP_TYPE ret_fperr; 
      bool update_global_best_conf = false;     
      if (lpErr == 0 && hpErr == 0) {
	update_global_best_conf = 
	  UpdateRTReport (eva_basis, 
			  this_vLPs, this_vHPs, 
			  eva_basis.getInputname().c_str(), 
			  ret_fperr); 
      }
      
      string my_inputname = eva_basis.getInputname();
            
      if (update_global_best_conf) {
	GLOBAL_BEST_CONF = conf;
	N_GLOBAL_UPDATES++; 	
      }
    }

    // check is timeout 
    N_RTS += eva_basis.getNInputRepeats(); 
    if (isTimeout(TSTART)) break; // return local_best;
  }

  return local_best;
}


// if nn_to_explore == -1 -->> explore all neighbors 
void IterativeFirstImprovement (CONF conf, CONF &ret, RTReport &ret_report, EvaluationBasis eva_basis, long nn_to_explore) {
  assert(nn_to_explore == -1 || 
	 nn_to_explore > 0);
  ret = conf;
  ret_report = grtEvaluateCONF(eva_basis, N_SEARCH_ITERATIONS, ret);
  vector<CONF> neighbors; 
  if (nn_to_explore < 0) {
    neighborCONFs(neighbors, conf);
    nn_to_explore = neighbors.size();
  }
  else { 
    for (int ni = 0 ; ni < nn_to_explore ; ni++) 
      neighborCONFs(neighbors, conf, true);
    assert(neighbors.size() == nn_to_explore);
  }
  for (unsigned int ni = 0 ; ni < nn_to_explore ; ni++) {
    unsigned int nid = rand() % neighbors.size();
    CONF neighbor_conf = neighbors[nid];    
    RTReport neighbor_report = grtEvaluateCONF(eva_basis, N_SEARCH_ITERATIONS, neighbor_conf);
    if (RTReportCompare(ret_report, neighbor_report) > 0) {
      ret = neighbor_conf;
      ret_report = neighbor_report;
    }
    neighbors.erase(neighbors.begin() + nid);
  }
}


// ---- begin of routines for my algo. ----

void complementCONF (CONF base, CONF dsub, CONF &dret) {
  unsigned int dsize = base.size();
  assert(dret.size() == 0);
  assert(dsize == dsub.size());
  for (unsigned int vi = 0 ; vi < dsize ; vi++) {
    assert(base[vi].first == vi && dsub[vi].first == vi);
    assert(base[vi].second.n_pars == 1 && base[vi].second.index_pars == 0);
    assert(dsub[vi].second.n_pars == 1 && base[vi].second.index_pars == 0);
    if (base[vi].second.ub == dsub[vi].second.ub) {
      assert(base[vi].second.lb <= dsub[vi].second.lb); 
      FPR new_fpr (base[vi].second.lb, dsub[vi].second.lb, 0, 1);
      dret.push_back(pair<unsigned int, FPR>(vi, new_fpr));
    }
    else if (base[vi].second.lb == dsub[vi].second.lb) {
      assert(base[vi].second.ub >= dsub[vi].second.ub);
      FPR new_fpr (dsub[vi].second.ub, base[vi].second.ub, 0, 1);
      dret.push_back(pair<unsigned int, FPR>(vi, new_fpr));
    }
    else {
      cerr << "ERROR: Invalid Arguments of complementCONF" << endl;
      assert(false);
    }
  }
}

// if select == 'r' -> random 
// if select == 'u' -> all upper binary sub. 
// if select == 'l' -> all lower binary sub. 
// otherwise -> error 
void binarySubCONF (CONF base, CONF &dret, char select) {
  assert(dret.size() == 0); 
  assert(select == 'r' || select == 'u' || select == 'l'); 
  unsigned int dsize = base.size();
  for (unsigned int vi = 0 ; vi < dsize ; vi++) {
    assert(base[vi].first == vi);
    HFP_TYPE mb = (base[vi].second.ub + base[vi].second.lb) / 2.0; 
    char uorl = (select == 'r') ? 
      ((rand() % 2 == 0) ? 'u' : 'l') : 
      select ;
    if (uorl == 'l') {
      FPR new_fpr (base[vi].second.lb, mb, 0, 1);
      dret.push_back(pair<unsigned int, FPR>(vi, new_fpr));
    }
    else { // uorl == 'u'
      FPR new_fpr (mb, base[vi].second.ub, 0, 1);
      dret.push_back(pair<unsigned int, FPR>(vi, new_fpr));
    }
  }
}

// will return (n_rand * 2 + 2) confs 
void generateBinarySubCONFs (CONF base, vector<CONF> &ret_confs, unsigned int n_rand) {
  assert(ret_confs.size() == 0);
  CONF all_upper; binarySubCONF(base, all_upper, 'u');
  CONF all_lower; binarySubCONF(base, all_lower, 'l');
  ret_confs.push_back(all_upper);
  ret_confs.push_back(all_lower);

  for (unsigned int di = 0 ; di < n_rand ; di++) {
    CONF new_conf0; binarySubCONF(base, new_conf0, 'r');
    CONF new_conf1; complementCONF(base, new_conf0, new_conf1); 
    ret_confs.push_back(new_conf0);
    ret_confs.push_back(new_conf1);
  }
}

// will try (n_rand * 2 + 2) confs 
void BinaryPartitionImprovement (CONF conf, CONF &ret_conf, EvaluationBasis eva_basis, unsigned int n_rand, bool unguided) {
  vector<CONF> cands; 
  
  generateBinarySubCONFs(conf, cands, n_rand);
  assert(cands.size() == (n_rand * 2 + 2));

  RTReport curr_best = grtEvaluateCONF(eva_basis, N_SEARCH_ITERATIONS, cands[0]);
  ret_conf = cands[0]; 

  for (unsigned int di = 1 ; di < cands.size() ; di++) {
    RTReport this_best = grtEvaluateCONF(eva_basis, N_SEARCH_ITERATIONS, cands[di]);
    if (RTReportCompare(curr_best, this_best) > 0) {
      curr_best = this_best;
      ret_conf = cands[di];
    }
  }

  if (unguided) 
    ret_conf = cands[rand() % cands.size()]; 
}

// ---- end of routines for my algo. ---- 


bool isLittleEndian() {
  short int n = 0x1;
  char *np = (char*)&n;
  return (np[0] == 1);
}


// return 1 for true
// return 0 for false
template <class FT> 
bool isValidFP (FT in_fp) {
  assert(sizeof(FT)==4 || sizeof(FT)==8 || sizeof(FT)==16);
  if (sizeof(FT) == 4) {
    uint32_t fp;
    memcpy(&fp, &in_fp, 4);
    uint64_t mant = fp & 0x7FFFFF;
    int64_t expo = (fp >> 23) & 0xFF;
    int64_t sign = (((fp >> 23) & 0x100) >> 8);
    if (expo == 255) return false;
  }
  else if (sizeof(FT) == 8) {
    uint64_t fp;
    memcpy(&fp, &in_fp, 8);
    uint64_t mant = fp & 0xFFFFFFFFFFFFF;
    int64_t expo = (fp >> 52) & 0x7FF;
    int64_t sign = (((fp >> 52) & 0x800) >> 11);
    if (expo == 2047) return false;
  }
  else { // sizeof(FT) == 16
    uint64_t fp[2]; 
    memcpy(&fp, &in_fp, 16);
    uint64_t fpM;
    uint64_t fpL;
    if (isLittleEndian()) {
      fpL = fp[0];
      fpM = fp[1];
    }
    else {
      fpM = fp[0];
      fpL = fp[1];
    }
    uint64_t mantM = fpM & 0xFFFFFFFFFFFF;
    int64_t expo = (fpM >> 48) & 0x7FFF;
    int64_t sign = (((fpM >> 48) & 0x8000) >> 15);
    if (expo == 32767) return false;
  }
  return true;
}


void reportResult (const char *tname) {

  /* 
     Output to console
  */ 
  cout << "N VALID " << tname << " TESTS: " << N_VALID_RTS << endl;
  cout << "N " << tname << " TESTS: " << N_RTS << endl;
  cout << "N LOCAL UPDATES: " << N_LOCAL_UPDATES << endl;
  cout << "N GLOBAL UPDATES: " << N_GLOBAL_UPDATES << endl;
  if (HALT_NOW) {
    time_t tend; 
    time(&tend);    
    cout << "HALT EARLY: # of SVE : " << N_RTS << endl;
    cout << "HALT EARLY: time : " << (tend - TSTART) << endl;
  }
  GLOBAL_BEST.dumpToStdout();   
  GLOBAL_BEST.dumpBestInputToFile (BEST_INPUT_FILE_NAME_LB, ID_LB);
  GLOBAL_BEST.dumpBestInputToFile (BEST_INPUT_FILE_NAME_UB, ID_UB);

  // output statistical data
  HFP_TYPE error_ave = 0.0;
  HFP_TYPE error_var = 0.0;
  error_ave = ERROR_SUM / (HFP_TYPE)N_VALID_RTS;
  error_var = (ERROR_SUM_2 / (HFP_TYPE)N_VALID_RTS) - (error_ave * error_ave); 

  cout << "AVERAGE ERROR: " << (CONSOLE_OUTPUT_TYPE) error_ave << endl;
  cout << "ERROR VARIANCE: " << (CONSOLE_OUTPUT_TYPE) error_var << endl;

  // write unstable report 
  if (CHECK_UNSTABLE_ERROR && (HALT_NOW == false)) {
    fprintf(unstable_report, "0 0\n"); 
  }



  /* 
     Output to result log 
  */
  const char *logname = "./s3fp_results.csv"; 
  FILE *flog = NULL; 
  
  // check if the file exist and open it 
  flog = fopen(logname, "r"); 
  if (flog == NULL) { // the file doesn't exist 
    flog = fopen(logname, "w"); 
    fprintf(flog, "\"Log Time\","); 
    fprintf(flog, "\"Error Func.\","); 
    fprintf(flog, "\"Average Error\","); 
    fprintf(flog, "\"Error LB\","); 
    fprintf(flog, "\"Error UB\","); 
    fprintf(flog, "\"Abs. Worst\","); 
    fprintf(flog, "\"Elapse Time\","); 
    fprintf(flog, "\"# SVE\"\n"); 
  }
  else { // the file exists 
    fclose(flog); 
    flog = fopen(logname, "a"); 
  }

  assert(flog != NULL); 

  // write log time 
  time_t log_time; 
  time( &log_time ); 
  
  struct tm * tm_log_time = localtime( &log_time ); 
  
  string str_log_time( asctime(tm_log_time) ); 
  if (str_log_time[str_log_time.length()-1] == '\n') 
    str_log_time[str_log_time.length()-1] = '\0'; 
 
  fprintf(flog, "\"%s\",", str_log_time.c_str()); 

  // write error func 
  switch(ERR_FUNC) {
  case REL_ERR_FUNC: 
    fprintf(flog, "\"REL\","); 
    break; 
  case ABSREL_ERR_FUNC: 
    fprintf(flog, "\"ABSREL\","); 
    break; 
  case ABSRELMAXONE_ERR_FUNC: 
    fprintf(flog, "\"ABSRELMAXONE\","); 
    break; 
  case ABS_ERR_FUNC: 
    fprintf(flog, "\"ABS\","); 
    break; 
  case ABSABS_ERR_FUNC: 
    fprintf(flog, "\"ABSABS\","); 
    break; 
  case LOWP_ERR_FUNC:
    fprintf(flog, "\"LOWP\","); 
    break; 
  default: 
    assert("ERROR: invalid ERR_FUNC..."); 
  }
  
  // write average error 
  fprintf(flog, "%11.10f,", (double) error_ave); 
  
  // write error LB 
  assert(GLOBAL_BEST.has_best_error); 
  double worst_lb = GLOBAL_BEST.best_error.lb; 
  fprintf(flog, "%11.10f,", worst_lb); 
  
  // write error UB 
  assert(GLOBAL_BEST.has_best_error); 
  double worst_ub = GLOBAL_BEST.best_error.ub; 
  fprintf(flog, "%11.10f,", worst_ub); 
  
  // write absolute worst-case error 
  double abs_worst_ub = (worst_ub >= 0.0) ? worst_ub : (-1.0 * worst_ub);  
  double abs_worst_lb = (worst_lb >= 0.0) ? worst_lb : (-1.0 * worst_lb); 
  double abs_worst = (abs_worst_ub >= abs_worst_lb) ? abs_worst_ub : abs_worst_lb ; 
  fprintf(flog, "%11.10f,", abs_worst); 
  
  // write elapse time 
  time_t tend; 
  time(&tend);
  fprintf(flog, "%d,", (tend - TSTART)); 

  // write # SVE
  fprintf(flog, "%d\n", N_RTS); 

  // close file 
  fclose(flog); 
}


// ======== begin of plain RT functions =========
void rtPlainRT (EvaluationBasis eva_basis, unsigned int n_vars) {
  CONF plain_conf; randCONF(plain_conf, n_vars, true);
  grtEvaluateCONF(eva_basis, 0, plain_conf);

  reportResult("PLAIN");
}
// ======== end of plain RT functions =========


// ========= begin of BGRT functions ========
void rtBGRT (EvaluationBasis eva_basis, unsigned int n_vars) {
  CONF top_conf; randCONF(top_conf, n_vars, true);
  CONF init_conf = top_conf;
  CONF curr_subconf = top_conf;
  
  while (!TerminationCriterion(TSTART)) {
    // dumpCONFtoStdout(curr_subconf);
    BinaryPartitionImprovement(curr_subconf, curr_subconf, eva_basis, N_RANDOM_BINARY_PARTITIONS); 

    bool go_restart = false;
    go_restart = toRestart(N_RESTARTS);

    if (go_restart) {
      curr_subconf = init_conf;    
    }
  }

  reportResult("BGRT");
}
// ========= end of BGRT functions =======


// ======== begin of ILS functions ========
void rtILS (EvaluationBasis eva_basis, unsigned int n_vars ) {
  // init the GLOBAL_BEST_CONF 
  CONF local_conf; randCONF(local_conf, n_vars);
  RTReport local_err = grtEvaluateCONF(eva_basis, N_SEARCH_ITERATIONS, local_conf);

  for (unsigned int r = 0 ; r < ILSr ; r++) {
    CONF curr_conf; randCONF(curr_conf, n_vars);
    RTReport curr_err = grtEvaluateCONF(eva_basis, N_SEARCH_ITERATIONS, curr_conf);
    if (RTReportCompare(local_err, curr_err) > 0) {
      local_conf = curr_conf;
      local_err = curr_err;
    }
  }
  cout << "--- finished the random start ----" << endl;

  IterativeFirstImprovement(local_conf, local_conf, local_err, 
			    eva_basis, ILSifi); // -1);

  cout << "--- ILS finished phase 1 ---- " << endl;
  while (!TerminationCriterion(TSTART)) {
    CONF curr_conf = local_conf;
    RTReport curr_err = local_err;
    vector<CONF> neighbors; 
    for (unsigned int s = 0 ; s < ILSs ; s++) {
      neighbors.clear();
      /*
      neighborCONFs (neighbors, curr_conf); 
      assert(neighbors.size() > 0); 
      curr_conf = neighbors[rand()% neighbors.size()];
      */
      neighborCONFs (neighbors, curr_conf, true);
      assert(neighbors.size() == 1);
      curr_conf = neighbors[0];
    }

    IterativeFirstImprovement(curr_conf, local_conf, local_err, 
			      eva_basis, ILSifi); // -1); 

    if (toRestart(N_RESTARTS)) {
      randCONF(local_conf, n_vars);
    }
  }

  reportResult("ILS"); 
}
// ======== end of ILS functions ========


// ======== begin of PSO functions ========
typedef struct {
  CONF conf;
  FPR err_range; 
} PARTICLE;
typedef vector<PARTICLE> SWARM; 
SWARM CURR_SWARM;

void dumpSWARMtoStdout (SWARM this_swarm) {
  SWARM::iterator siter = this_swarm.begin();
  for ( ; siter != this_swarm.end() ; siter++) {
    dumpCONFtoStdout(siter->conf);
  }
}

void randSWARM (unsigned int n_particles, unsigned int n_vars) {
  for (int pi = 0 ; pi < n_particles ; pi++) {
    CONF new_conf;
    randCONF(new_conf, n_vars, false);
    PARTICLE new_part;
    new_part.conf = new_conf;
    new_part.err_range.lb = 1; new_part.err_range.ub = -1; // the initail range is an invalid range
    CURR_SWARM.push_back(new_part);
  }
}
  
void grtEvaluateSWARM (EvaluationBasis eva_basis, unsigned int N) {
  SWARM next_swarm; 

  // evaluate particles and rank them 
  SWARM::iterator csiter = CURR_SWARM.begin();
  for ( ; csiter != CURR_SWARM.end() ; csiter++) {
    RTReport part_report = grtEvaluateCONF(eva_basis, N, csiter->conf);
    if (isTimeout(TSTART)) return; 
    csiter->err_range = part_report.best_error;

    SWARM::iterator nsiter = next_swarm.begin();
    for ( ; nsiter != next_swarm.end() ; nsiter++) {
      if (FPRCompare(nsiter->err_range, csiter->err_range) > 0) {
	next_swarm.insert(nsiter, (*csiter));
	break;
      }
    }
    if (nsiter == next_swarm.end())
      next_swarm.push_back((*csiter));
  }

  // move particles 
  unsigned int velocity_points = 0; 
  SWARM::iterator nsiter = next_swarm.begin();
  for ( ; nsiter != next_swarm.end() ; nsiter++) {
    for (unsigned int vi = 0 ; vi < velocity_points ; vi++) {
      unsigned int direction = rand() % nsiter->conf.size();
      if (rand() % 2 == 0)
	nsiter->conf[direction].second.index_pars++;
      else 
	nsiter->conf[direction].second.index_pars--;
      if (nsiter->conf[direction].second.index_pars < 0) 
	nsiter->conf[direction].second.index_pars = 0;
      if (nsiter->conf[direction].second.index_pars >= nsiter->conf[direction].second.n_pars)
	nsiter->conf[direction].second.index_pars = nsiter->conf[direction].second.n_pars - 1;
    }
    velocity_points += nsiter->conf.size();
  }
 
  CURR_SWARM.clear();
  CURR_SWARM = next_swarm; 
}

void rtPSO (EvaluationBasis eva_basis, unsigned int n_particles, unsigned int n_vars, unsigned int N) {
  randSWARM(n_particles, n_vars);
  
  while(!TerminationCriterion(TSTART)) {
    grtEvaluateSWARM(eva_basis, N);
  }

  reportResult("PSO");
}
// ======== end of PSO functions ========
  

// ========= begin of BURT functions ========
void rtBURT (EvaluationBasis eva_basis, unsigned int n_vars) {
  CONF init_conf; randCONF(init_conf, n_vars, true); 
  CONF curr_subconf = init_conf; 
  
  while (!TerminationCriterion(TSTART)) {
    BinaryPartitionImprovement(curr_subconf, curr_subconf, eva_basis, N_RANDOM_BINARY_PARTITIONS, true); 

    bool go_restart = false;
    go_restart = toRestart(N_RESTARTS);

    if (go_restart) {
      curr_subconf = init_conf;    
    }
  }

  reportResult("BURT");
}
// ========= end of BGRT functions =======

// ======== begin of Opt4j function ========
void runOpt4jInput (EvaluationBasis eva_basis, unsigned int n_vars, const char *iname) {

  long fsize; 
  CONF this_conf; 
  bool updated_global = false;

  FILE *infile = fopen(iname, "r"); 
  if (infile == NULL) return; 
  fseek(infile, 0, SEEK_END); 
  fsize = ftell(infile);
  assert(fsize == (sizeof(INPUTV_TYPE) * n_vars)); 
  fseek(infile, 0, SEEK_SET); 
 
  for (int i = 0 ; i < n_vars ; i++) 
    this_conf.push_back(pair<unsigned int, FPR>(i, FPR(0, 0, 0, 1)));

  int lperr, hperr; 
  vector<HFP_TYPE> lp_outputs; 
  vector<HFP_TYPE> hp_outputs;
  HFP_TYPE ret_fperr; 
  
  for (int i = 0 ; i < n_vars ; i++) {
    INPUTV_TYPE this_fp; 
    fread(&this_fp, sizeof(INPUTV_TYPE), 1, infile); 
    this_conf[i].second = FPR(this_fp, this_fp, 0, 1); 
  }
  fclose(infile); 

  assert(!backdoor_use_external_input); 
  eva_basis.prepareInput(this_conf); 
  
  lp_outputs.clear(); hp_outputs.clear(); 
  eva_basis.runLP(&lperr, lp_outputs);
  eva_basis.runHP(&hperr, hp_outputs); 
  // assert(lperr != 0 && hperr != 0); // opt4j may be manually terminated
  assert(lp_outputs.size() > 0 && hp_outputs.size() > 0);

  updated_global = UpdateRTReport(eva_basis, 
				  lp_outputs, hp_outputs, 
				  iname, 
				  ret_fperr); 
}

void rtOpt4j (EvaluationBasis eva_basis, unsigned int n_vars) {
  const char *opt4j_exe = "/home/wfchiang/tools/opt4j-3.0.1/bin/opt4j"; 
  const char *lb_input_filename = "opt4j_best_input_lb"; 
  const char *lb_input_filename_main = "opt4j_best_input_lb_main"; 
  const char *ub_input_filename = "opt4j_best_input_ub"; 
  const char *ub_input_filename_main = "opt4j_best_input_ub_main"; 
  const char *div_input_filename = "opt4j_div_input"; 
  const char *div_input_filename_main = "opt4j_div_input_main"; 
  stringstream ss; 

  // -- remove opt4j's reports -- 
  ss.str(""); ss.clear(); 
  ss << "rm " << eva_basis.getInputname() << endl; 
  system(ss.str().c_str()); 

  ss.str(""); ss.clear(); 
  ss << "rm " << eva_basis.getInputname() << "-main" << endl; 
  system(ss.str().c_str()); 

  ss.str(""); ss.clear(); 
  ss << "rm " << lb_input_filename << endl; 
  system(ss.str().c_str()); 

  ss.str(""); ss.clear(); 
  ss << "rm " << lb_input_filename_main << endl; 
  system(ss.str().c_str()); 

  ss.str(""); ss.clear(); 
  ss << "rm " << ub_input_filename << endl;
  system(ss.str().c_str()); 

  ss.str(""); ss.clear(); 
  ss << "rm " << ub_input_filename_main << endl;
  system(ss.str().c_str()); 

  ss.str(""); ss.clear(); 
  ss << "rm " << div_input_filename << endl;
  system(ss.str().c_str()); 

  ss.str(""); ss.clear(); 
  ss << "rm " << div_input_filename_main << endl;
  system(ss.str().c_str()); 

  // -- run opt4j -- 
  ss.str(""); ss.clear(); 
  ss << opt4j_exe << " -s opt4j-config.xml" << endl;
  system(ss.str().c_str()); 

  // ---- run with the lb and the hb inputs ---- 
  
  cout << "-- run opt4j input : " << ub_input_filename << " --" << endl; 
  runOpt4jInput (eva_basis, n_vars, ub_input_filename); 
  cout << "-- run opt4j input : " << ub_input_filename_main << " --" << endl;
  runOpt4jInput (eva_basis, n_vars, ub_input_filename_main); 
  cout << "-- run opt4j input : " << lb_input_filename << " --" << endl;
  runOpt4jInput (eva_basis, n_vars, lb_input_filename); 
  cout << "-- run opt4j input : " << lb_input_filename_main << " --" << endl;
  runOpt4jInput (eva_basis, n_vars, lb_input_filename_main);

  // in case of div. detected 
  if (CHECK_UNSTABLE_ERROR) {
    FILE *dfile = fopen(div_input_filename, "r"); 
    if (dfile != NULL) {
      ss.str(""); ss.clear(); 
      ss << "cp " << div_input_filename << " " << eva_basis.getInputname() << endl;
      system(ss.str().c_str()); 

      ss.str(""); ss.clear(); 
      ss << "cp " << div_input_filename << " " << eva_basis.getInputname() << "_main" << endl;
      system(ss.str().c_str()); 

      HALT_NOW = true; 
    }
    fclose(dfile); 
  }

  reportResult("Opt4j");  
} 

// ======== end of Opt4j function ========

// ======== begin of uniform robustness check ========
void uniformRobustnessCheck (EvaluationBasis eva_basis, unsigned int n_vars) {
  CONF init_conf; randCONF(init_conf, n_vars, true); 

  unsigned int i = 0; 
  int lperr, hperr; 
  vector<HFP_TYPE> lp_outputs; 
  vector<HFP_TYPE> hp_outputs; 

  unsigned int n_divs = 0; 

  while (!TerminationCriterion(TSTART)) {    lp_outputs.clear(); 
    hp_outputs.clear(); 
    
    assert(!backdoor_use_external_input); 
    eva_basis.prepareInput(init_conf); 

    eva_basis.runLP(&lperr, lp_outputs); 
    eva_basis.runHP(&hperr, hp_outputs); 
    assert(lperr == 0 && hperr == 0); 
    assert(lp_outputs.size() != 0); 
    assert(hp_outputs.size() != 0); 

    vector<HFP_TYPE> lp_err_est_vals; 
    vector<HFP_TYPE> hp_err_est_vals; 
    vector<HFP_TYPE> lp_div_det_vals;
    vector<HFP_TYPE> hp_div_det_vals; 
    ExtractEEstSigValues(lp_outputs, lp_err_est_vals, lp_div_det_vals); 
    ExtractEEstSigValues(hp_outputs, hp_err_est_vals, hp_div_det_vals); 

    N_RTS++; 
    bool got_div = IsDivergenceTriggered(lp_div_det_vals, 
					 hp_div_det_vals, 
					 DIFF_CON, NULL); 
    if (got_div) {
      n_divs++; 
      cout << "Div. detected: " << n_divs << " / " << N_RTS << endl;

    }
  }

  cout << "# Divs: " << n_divs << endl;
  cout << "# Samples: " << N_RTS << endl;
  
  __float128 fp_n_divs = (__float128) n_divs; 
  __float128 fp_n_samples = (__float128) N_RTS;

  printf("Div. Rate: %31.30f \n", (double) (fp_n_divs / fp_n_samples)); 
}
// ======== end of uniform robustness check ========


// ======== begin of experimental analog white box sampling ========
typedef pair< pair<vector<INPUTV_TYPE>, vector<HFP_TYPE> >, pair<vector<INPUTV_TYPE>, vector<HFP_TYPE> > > AWBS_EDGE; 

// return true if an divergence was found 
// return false if no divergence was found 
bool analogWhiteBoxSampling (EvaluationBasis eva_basis, 
			     unsigned int n_vars) {
  assert(N_INPUT_REPEATS == 1); 
  assert(!backdoor_use_external_input); 

  CONF plain_conf; randCONF(plain_conf, n_vars, true);
  vector<AWBS_EDGE> sampling_edges; 

  vector<INPUTV_TYPE> inputA; 
  vector<INPUTV_TYPE> inputB; 
  vector<INPUTV_TYPE> inputC; 

  vector<HFP_TYPE> lp_errA; 
  vector<HFP_TYPE> lp_sigA; 
  vector<HFP_TYPE> hp_errA;
  vector<HFP_TYPE> hp_sigA;

  vector<HFP_TYPE> lp_errB; 
  vector<HFP_TYPE> lp_sigB; 
  vector<HFP_TYPE> hp_errB; 
  vector<HFP_TYPE> hp_sigB; 

  vector<HFP_TYPE> lp_errC; 
  vector<HFP_TYPE> lp_sigC; 
  vector<HFP_TYPE> hp_errC; 
  vector<HFP_TYPE> hp_sigC; 

  int lperr, hperr; 
  CONF confA; 
  CONF confB; 
  CONF confC; 

  // ==== find the initial edge ==== 
  if (AWBS_FIXED_INITIALA) {
    cout << "Fixed Initial A... to all -1" << endl;
    CONF fixed_initA_conf; 
    for (unsigned int i = 0 ; i < n_vars ; i++) {
      fixed_initA_conf.push_back(pair<unsigned int, FPR>
				 (i, 
				  // FPR(-1, -1, 0, 1)));
				  FPR(1, 1, 0, 1)));
				  // FPR(9, 9, 0, 1)));
    }
    eva_basis.prepareInput(fixed_initA_conf); 
  }
  else {
    cout << "Try to find the initial inputs..." << endl;
    eva_basis.prepareInput(plain_conf); 
  }

  if (AWBS_FIVESTAGE_ASSIST && 
      SIG_FUNC == FIVESTAGE_SIG_FUNC) {
    do {
      eva_basis.prepareInput(plain_conf); 

      eva_basis.runLP(&lperr, lp_errA, lp_sigA); 
    
      eva_basis.runHP(&hperr, hp_errA, hp_sigA); 

      N_RTS++; 

      assert(lp_sigA.size() == 1 && hp_sigA.size() == 1); 

      // cout << "FIVESTAGE find A..." << endl;
    } while ((lp_sigA[0] != hp_sigA[0]) || 
	     (lp_sigA[0] != 1.0)); 

    inputA.clear(); 
    eva_basis.getInputValues(inputA, N_INPUT_REPEATS);   
  }
  else { 
    inputA.clear(); 
    eva_basis.getInputValues(inputA, N_INPUT_REPEATS); 

    eva_basis.runLP(&lperr, lp_errA, lp_sigA); 
    
    eva_basis.runHP(&hperr, hp_errA, hp_sigA); 

    N_RTS++; 
  }

  if (IsDivergenceTriggered(lp_sigA, hp_sigA, 
			    DIFF_CON, NULL)) {
    cout << "Div. Detected after : " << N_RTS << " samples" << endl;
    return true; 
  }

  while (!TerminationCriterion(TSTART)) {
    eva_basis.prepareInput(plain_conf); 
    
    eva_basis.runLP(&lperr, lp_errB, lp_sigB); 
    
    eva_basis.runHP(&hperr, hp_errB, hp_sigB); 

    N_RTS++; 
    
    if (IsDivergenceTriggered(lp_sigB, hp_sigB, 
			      DIFF_CON, NULL)) {
      cout << "Div. Detected after : " << N_RTS << " samples" << endl;
      return true; 
    }

    if (AWBS_FIVESTAGE_ASSIST && 
	SIG_FUNC == FIVESTAGE_SIG_FUNC) {
      // cout << "FIVESTAGE find B... " << (float) lp_sigB[0] << " vs " << (float) hp_sigB[0] << endl;
      assert(lp_sigB.size() == 1 && hp_sigB.size() == 1); 
      if ((lp_sigB[0] != hp_sigB[0]) || 
	  (lp_sigB[0] != 2.0)) continue; 
    }
    
    bool traverse_ab = IsDifferentialContractViolated(lp_sigA, 
						      lp_sigB, 
						      SIG_FUNC, 
						      NULL); 

    if (traverse_ab) {
      eva_basis.getInputValues(inputB, N_INPUT_REPEATS); 
      sampling_edges.push_back(AWBS_EDGE(pair< vector<INPUTV_TYPE>, vector<HFP_TYPE> >(inputA, lp_sigA), 
					 pair< vector<INPUTV_TYPE>, vector<HFP_TYPE> >(inputB, lp_sigB))); 
      break; 
    }
  }
  if (TerminationCriterion(TSTART)) {
    cout << "Cannot detect div. within timeout (" << N_RTS << " samples)" << endl;
    return false;
  }
  cout << "Initial inputs found!" << endl;

  // loop until a divergence found 
  while (sampling_edges.size() > 0) {
    HFP_TYPE abdis = 0; 
    HFP_TYPE acdis = 0; 
    HFP_TYPE bcdis = 0; 

    // reset 
    inputA.clear(); inputB.clear(); inputC.clear(); 
    lp_errA.clear(); lp_errB.clear(); lp_errC.clear(); 
    lp_sigA.clear(); lp_sigB.clear(); lp_sigC.clear(); 
    hp_errA.clear(); hp_errB.clear(); hp_errC.clear(); 
    hp_sigA.clear(); hp_sigB.clear(); hp_sigC.clear(); 
    confA.clear(); confB.clear(); confC.clear(); 

    inputA = sampling_edges[0].first.first;
    lp_sigA = sampling_edges[0].first.second; 
    inputB = sampling_edges[0].second.first;
    lp_sigB = sampling_edges[0].second.second; 

    sampling_edges.erase(sampling_edges.begin()); 
   
    // set inputC and confC 
    assert(inputA.size() == inputB.size()); 
    abdis = 0; 
    for (int i = 0 ; i < inputA.size() ; i++) {
      inputC.push_back((inputA[i] + inputB[i]) / 2); 
      abdis += (((HFP_TYPE)inputA[i] - (HFP_TYPE)inputB[i]) * 
		((HFP_TYPE)inputA[i] - (HFP_TYPE)inputB[i])); 
    }
    
#ifdef S3FP_VERBOSE 
    cout << "A <-> B : " << (CONSOLE_OUTPUT_TYPE) abdis << endl;
#endif 

    // check validity of inputC 
    bool validC = true; 

    assert(inputA.size() == inputC.size()); 
    for (int i = 0 ; i < inputC.size() ; i++) {
      if (inputA[i] <= inputC[i] && 
	  inputC[i] <= inputB[i]) ;  
      else if (inputB[i] <= inputC[i] && 
	       inputC[i] <= inputA[i]) ; 
      else {
	validC = false; 
	cout << ".... invalid output" << endl;
	break; 
      }
    }

    // run A, B, and C 
    if (validC) {
      // compute acdis and bcdis 
      acdis = bcdis = 0; 
      for (int i = 0 ; i < inputC.size() ; i++) {
	acdis += (((HFP_TYPE)inputA[i] - (HFP_TYPE)inputC[i]) * 
		 ((HFP_TYPE)inputA[i] - (HFP_TYPE)inputC[i])); 
	bcdis += (((HFP_TYPE)inputB[i] - (HFP_TYPE)inputC[i]) * 
		 ((HFP_TYPE)inputB[i] - (HFP_TYPE)inputC[i])); 
      }
      
      if (abdis <= acdis || 
	  abdis <= bcdis) continue; 

      // set confC 
      for (int i = 0 ; i < inputC.size() ; i++) 
	confC.push_back(pair<unsigned int, FPR>(i, 
						FPR(inputC[i], inputC[i], 0, 1))); 

      // run C lp and hp 
      eva_basis.prepareInput(confC); 
      eva_basis.runLP(&lperr, lp_errC, lp_sigC); 
      eva_basis.runHP(&hperr, hp_errC, hp_sigC); 

      N_RTS++; 

      if (IsDivergenceTriggered(lp_sigC, hp_sigC, 
				DIFF_CON, NULL)) {
	  cout << "Div. Detected after : " << N_RTS << " samples" << endl;
	  return true; 
      }
	
      bool traverse_ac = IsDifferentialContractViolated(lp_sigA, 
							lp_sigC,
							SIG_FUNC, 
							NULL); 
      bool traverse_bc = IsDifferentialContractViolated(lp_sigB, 
							lp_sigC,
							SIG_FUNC, 
							NULL); 
      
      if (traverse_ac) {
#ifdef S3FP_VERBOSE 
	cout << "    trace A <-> C (" << (CONSOLE_OUTPUT_TYPE) acdis << ")" << endl;
#endif 
	sampling_edges.push_back(AWBS_EDGE(pair< vector<INPUTV_TYPE>, vector<HFP_TYPE> >(inputA, lp_sigA), 
					   pair< vector<INPUTV_TYPE>, vector<HFP_TYPE> >(inputC, lp_sigC))); 
      }

      if (traverse_bc) {
#ifdef S3FP_VERBOSE 
	cout << "    trace B <-> C (" << (CONSOLE_OUTPUT_TYPE) bcdis << ")" << endl; 
#endif 
	sampling_edges.push_back(AWBS_EDGE(pair< vector<INPUTV_TYPE>, vector<HFP_TYPE> >(inputB, lp_sigB), 
					   pair< vector<INPUTV_TYPE>, vector<HFP_TYPE> >(inputC, lp_sigC))); 
      } 
    }
    
    if (TerminationCriterion(TSTART)) {
      cout << "Cannot detect div. within timeout (" << N_RTS << " samples)" << endl;
      return false;
    }
  }

  cout << "Failed to detect div. .... (" << N_RTS << " samples)... restart" << endl; 

  return false; 
}


void rtAnalogWhiteBoxSampling (EvaluationBasis eva_basis, 
			       unsigned int n_vars) {
  unsigned int n_restarts = 0; 
  bool success = false; 
  while (!TerminationCriterion(TSTART)) {
    success = analogWhiteBoxSampling(eva_basis, n_vars); 

    if (success) break; 
    else n_restarts++; 
  }
  if (success) {
    time_t tend; time(&tend); 
    cout << "Found a div. in time: " << (tend - TSTART) << endl; 
    cout << "# Restarts: " << n_restarts << endl;
  }
}
// ======== end of experimental analog white box sampling ========


// ======== begin of error magnification Test ========
/*
  pert_type: 
  "relerr" : relative error 
  "abserr" : absolute error 
*/
void inputPerturbation (vector<INPUTV_TYPE> base_input,
			vector<INPUTV_TYPE> &pert_input, 
			string pert_type, 
			HFP_TYPE max_pert) {
  assert(base_input.size() > 0); 
  assert(pert_input.size() == 0);
  assert(max_pert >= 0); 

  unsigned int i = 0; 
  
  if (pert_type.compare("relerr") == 0) {
    for (i = 0 ; i < base_input.size() ; i++) {
      double pert = (double) random128(-1, 1, NAIVE_RANDOM_FUNC); 
      pert_input.push_back((INPUTV_TYPE)((1.0 + pert * max_pert) * base_input[i])); 
    }
  }
  else if (pert_type.compare("abserr") == 0) {
    for (i = 0 ; i < base_input.size() ; i++) {
      double pert = (double) random128(-1 * max_pert, max_pert, 
				       NAIVE_RANDOM_FUNC); 
      pert_input.push_back((INPUTV_TYPE)(base_input[i] + pert)); 
    }
  }
  else assert(false && "Error: Invalid perturbation type"); 

#ifdef S3FP_VERBOSE
  cout << "---- input comparison ----" << endl;
  for (i = 0 ; i < base_input.size() ; i++) 
    cout << (CONSOLE_OUTPUT_TYPE) base_input[i] << "  vs  " << (CONSOLE_OUTPUT_TYPE) pert_input[i] << "  (diff: " << (CONSOLE_OUTPUT_TYPE) (base_input[i] - pert_input[i]) << ")" << endl;
  cout << "--------------------------" << endl;
#endif 

  assert(base_input.size() == pert_input.size()); 
}

// ======== end of error magnification Test ========
