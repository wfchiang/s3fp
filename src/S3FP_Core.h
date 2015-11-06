#include "definitions.h"
#include "EvaluationBasis.h"
#include "S3FP_ParseArgs.h"

using namespace std;

#ifndef S3FP_Core 
#define S3FP_Core 


// ==== Global Variables for s3fp settings ====
unsigned int N_VARS = 0; 
// input file name 
string INPUT_FILE_NAME = ""; 
// randomg testing mode 
ENUM_RT_MODE RT_MODE = NA_RT_MODE;
// max. resource (timeout) 
unsigned int TIMEOUT = 0;
// type of resource (time, sve, or ?) 
ENUM_TRESOURCE T_RESOURCE = NA_TRESOURCE;
// executable LP
string EXE_LP_NAME = ""; 
// output LP 
string OUTPUT_LP_NAME = ""; 
// executable HP
string EXE_HP_NAME = ""; 
// output HP 
string OUTPUT_HP_NAME = ""; 
// seed for random number generator 
unsigned int RSEED = 0;
// the error optimization 
ENUM_ERR_OPT ERR_OPT = NA_ERR_OPT; 
// the error function 
ENUM_ERR_FUNC ERR_FUNC = NA_ERR_FUNC;
// signature function (differential contract) 
ENUM_SIG_FUNC SIG_FUNC = NA_SIG_FUNC; 
// this variable should actually be named as "DIV_FUNC" which check divergence (this is not always equal to differential contract) 
ENUM_SIG_FUNC DIFF_CON = NA_SIG_FUNC;
// given a range, how to find the representation number for opt. objective 
ENUM_OPT_REPRESENT OPT_REPRESENT = MIX_OPT_REPRESENT;
// indicate finding a wide range or a narrow rage 
ENUM_OPT_RANGE OPT_RANGE = WIDE_OPT_RANGE; 
// delta for calculating relative error 
HFP_TYPE REL_DELTA = 0.0;
// indicate using uniform input range or not 
bool UNIFORM_INPUT = true;
// file name which describes variable ranges 
string INPUT_RANGE_FILE;
// the lower bound of uniform input range 
HFP_TYPE UNIFORM_INPUTLB = 0.0;
// the upper bound of uniform input range 
HFP_TYPE UNIFORM_INPUTUB = 0.0; 
// indicate checking unstable error or not 
bool CHECK_UNSTABLE_ERROR = false;
// the file name for reporting unstable error testing result 
string UNSTABLE_ERROR_REPORT = ""; 
// # of inputs repeated in the (generated) input file 
unsigned int N_INPUT_REPEATS = 1;
// the type for floating-point number shown in (console) output 
#define CONSOLE_OUTPUT_TYPE long double
// file name for recording the input which triggers the error upper bound 
#ifndef BEST_INPUT_FILE_NAME_UB
#define BEST_INPUT_FILE_NAME_UB "best_input_ub"
#endif 
// file name for recording the input which triggers the error lower bound 
#ifndef BEST_INPUT_FILE_NAME_LB
#define BEST_INPUT_FILE_NAME_LB "best_input_lb"
#endif 
// file name for recording the input which triggers unstable error 
#ifndef UNSTABLE_INPUT_FILE_NAME 
#define UNSTABLE_INPUT_FILE_NAME "unstable_input" 
#endif 
// random number function 
ENUM_RANDOM_FUNC RANDOM_FUNC = NAIVE_RANDOM_FUNC; 
/*
  experimental setting
*/
// for awbs 
bool AWBS_FIXED_INITIALA = false; 
bool AWBS_FIVESTAGE_ASSIST = false; 
string BACKUP_DIV_INPUT_FILE_NAME = "backup_div_input_file"; 
HFP_TYPE SORTING_DIV_THRESHOLD = -1; 


/* 
   class definitions
*/
// report of random testing 
class RTReport {
public: 
  unsigned int n_tests; 
  bool has_best_error;
  FPR best_error; 

  vector<INPUTV_TYPE> best_input_ub;
  vector<INPUTV_TYPE> best_input_lb;

  RTReport () { has_best_error = false; n_tests = 0; }
  void reset () {
    n_tests = 0;
    has_best_error = false;
    best_input_lb.clear();
    best_input_ub.clear();
  }

  // return ture if either upper of the lower bound is updated 
  // return false if no bound is updated 
  bool addError (HFP_TYPE rel, const char *input_name) {

    n_tests++;
    if (has_best_error == false) {
      has_best_error = true;
      FPR in_best_error(rel, rel, 0, 1);
      best_error = in_best_error;
      saveTheBestInput(input_name, ID_LB);
      saveTheBestInput(input_name, ID_UB);
      return true;
    }
    else {
      bool ret = false;
      bool update_upper = false;
      bool update_lower = false;

      switch (OPT_RANGE) {
      case WIDE_OPT_RANGE:
	update_upper = rel > best_error.ub;
	update_lower = rel < best_error.lb;
	break;
      case NARROW_OPT_RANGE:
	assert(best_error.ub >= best_error.lb); 

	if (best_error.ub == 0 && best_error.lb == 0) {
	  if (rel > 0) update_upper = true;
	  if (rel < 0) update_lower = true;
	}
	else if (best_error.ub > 0 && best_error.lb == 0) {
	  if ((rel > 0) && (rel < best_error.ub)) update_upper = true;
	  if (rel < 0) update_lower = true;
	}
	else if (best_error.ub == 0 && best_error.lb < 0) {
	  if (rel > 0) update_upper = true;
	  if ((rel < 0) && (rel > best_error.lb)) update_lower = true;
	}
	else if (best_error.ub > 0 && best_error.lb < 0) {
	  if ((rel > 0) && 
	      (rel < best_error.ub)) update_upper = true;
	  if ((rel < 0) && 
	      (rel > best_error.lb)) update_lower = true;
	}
	else if (best_error.ub > 0 && best_error.lb > 0) {
	  assert(best_error.ub == best_error.lb); 
	  if ((rel > 0) && 
	      (rel < best_error.ub)) {
	    update_upper = true;
	    update_lower = true;
	  }
	  if (rel <= 0) 
	    update_lower = true;
	}
	else if (best_error.ub < 0 && best_error.lb < 0) {
	  assert(best_error.ub == best_error.lb);
	  if (rel >= 0)
	    update_upper = true;
	  if ((rel < 0) && 
	      (rel > best_error.lb)) {
	    update_upper = true;
	    update_lower = true;
	  }
	}
	break;
      default:
	assert(false); 
	break;
      }
     
      // try to update upper bound 
      if (update_upper) {
	best_error.ub = rel;
	saveTheBestInput(input_name, ID_UB);
	ret = true;
      }
      // try to update lower bound
      if (update_lower) {
	best_error.lb = rel;
	saveTheBestInput(input_name, ID_LB);
	ret = true;
      }

      return ret; 
    }
  }
  RTReport &operator=(const RTReport &rhs) {
    if (this == &rhs) return *this;
    this->n_tests = rhs.n_tests;
    this->best_error = rhs.best_error;
    this->has_best_error = rhs.has_best_error;
    this->best_input_lb = rhs.best_input_lb;
    this->best_input_ub = rhs.best_input_ub;
    return *this;
  }
  void dumpToOStream (ostream &ost) {
    // ost << endl;
    if (has_best_error) {
      ost << "Best Error: [" << (CONSOLE_OUTPUT_TYPE) best_error.lb << ", " << (CONSOLE_OUTPUT_TYPE) best_error.ub << "]" << endl;
      ost << "vLP : vHP when Error = " << (CONSOLE_OUTPUT_TYPE) best_error.lb << endl; 
      ost << "vLP : vHP when Error = " << (CONSOLE_OUTPUT_TYPE) best_error.ub << endl; 
    }
    else ost << "Best Error: N/A" << endl;
  }
  void dumpToStdout () {
    dumpToOStream(cout);
  } 
  void dumpBestInputToFile (const char *outname, ENUM_UB_OR_LB ub_or_lb) {
    assert(outname != NULL);
    vector <INPUTV_TYPE> &best_input = ((ub_or_lb==ID_UB) ? best_input_ub : best_input_lb);
    FILE *outfile = fopen(outname, "w");
    assert(outfile != NULL);
    for (unsigned int ii = 0 ; ii < best_input.size() ; ii++) {
      INPUTV_TYPE out_data = best_input[ii];
      fwrite(&out_data, sizeof(INPUTV_TYPE), 1, outfile);
    }
    fclose(outfile); 
  }
private:
  HFP_TYPE absoluteError (FPR error) {
    HFP_TYPE absub = (error.ub >= 0) ? error.ub : (-1 * error.ub);
    HFP_TYPE abslb = (error.lb >= 0) ? error.lb : (-1 * error.lb);
    return (absub >= abslb) ? absub : abslb;
  }
  void saveTheBestInput (const char *input_name, ENUM_UB_OR_LB ub_or_lb) {
    vector<INPUTV_TYPE> &best_input = ((ub_or_lb==ID_UB) ? best_input_ub : best_input_lb);

    if (input_name == NULL) return; 
    best_input.clear();
    FILE *infile = fopen(input_name, "r");
    fseek(infile, 0, SEEK_END);
    unsigned int n_inputs = ftell(infile);
    assert(n_inputs % sizeof(INPUTV_TYPE) == 0);
    n_inputs = n_inputs / sizeof(INPUTV_TYPE);
    fseek(infile, 0, SEEK_SET);
    for (unsigned int ii = 0 ; ii < n_inputs ; ii++) {
      INPUTV_TYPE this_input;
      fread(&this_input, sizeof(INPUTV_TYPE), 1, infile);
      best_input.push_back(this_input);
    }
    fclose(infile);
  } 
}; 




class ThreadWorkQueue {
 public:
  EvaluationBasis t_eva_basis; 
  unsigned int t_N;
  vector<CONF> t_confs;
  bool t_use_cache;
  bool t_cache_local;
  pthread_mutex_t t_lock_exit;
  bool t_exit;
  unsigned int t_pid;
  unsigned int t_tid;

  ThreadWorkQueue () {}

  ThreadWorkQueue (EvaluationBasis in_eva_basis, unsigned int in_N, bool in_use_cache, bool in_cache_local, bool in_exit, unsigned int in_pid, unsigned int in_tid) {
    init(in_eva_basis, in_N, in_use_cache, in_cache_local, in_exit, 
	 in_pid, in_tid);
  }

  void init (EvaluationBasis in_eva_basis, unsigned int in_N, bool in_use_cache, bool in_cache_local, bool in_exit, unsigned int in_pid, unsigned int in_tid) {
    t_eva_basis = in_eva_basis;
    t_N = in_N;
    t_use_cache = in_use_cache;
    t_cache_local = in_cache_local;
    t_exit = in_exit;
    t_pid = in_pid;
    t_tid = in_tid;
  }

  void pushCONF (CONF in_conf) {
    CONF new_conf = in_conf;
    t_confs.push_back(new_conf);
  }

  void popCONF (CONF &ret_conf) {
    assert(t_confs.size() > 0);
    ret_conf = t_confs.front();
    t_confs.erase(t_confs.begin());
  }
};


// ==== global variables used in the execution of s3fp ==== 
// summation of all detected errors 
HFP_TYPE ERROR_SUM = 0.0;
// summation of all (detected error)^2 s 
HFP_TYPE ERROR_SUM_2 = 0.0;
// variance of detected errors 
HFP_TYPE ERROR_VAR = 0.0;
// # of random testing performed 
unsigned int N_RTS = 0;
// # of valid random testing performed 
unsigned int N_VALID_RTS = 0;
// the global best conf. detected so far
CONF GLOBAL_BEST_CONF; 
// the global best error detected so far 
RTReport GLOBAL_BEST;
// # of restarts 
unsigned int N_RESTARTS = 0;
// global starting time 
time_t TSTART;
// # of local updates 
unsigned int N_LOCAL_UPDATES = 0;
// # of global updates 
unsigned int N_GLOBAL_UPDATES = 0;
// local best errors 
RTReport local_best;
// out stream for conf. -> errors 
ofstream ost_conf_to_errors; 
// indicate whether an unstable error been detected or not (halt immediately when an unstable error is detected) 
bool HALT_NOW;
// output file for unstable error report 
FILE *unstable_report; 
// a switch of the back-door of using the external input 
bool backdoor_use_external_input = false; 


// ==== global variables for parallel random testing ==== 
#ifndef NTPERP // number of threads per process 
#define NTPERP 10 
#endif 
unsigned int _PID = 0;
unsigned int _TID = 0; 
pthread_t TPOOL[NTPERP];
unsigned int TIDS[NTPERP];
ThreadWorkQueue TWQ[NTPERP]; 
pthread_mutex_t lock_local_best;
pthread_mutex_t lock_inputgen;
pthread_barrier_t TBARR; 
pthread_barrierattr_t TBATTR;


// ==== global variables for ILS's parameters ==== 
unsigned int ILSifi = 20;
unsigned int ILSr = 4;
unsigned int ILSs = 50;


// ==== global variables for PSO's parameters ==== 
unsigned int SWARM_SIZE = 20; 


// ==== global variable for settings for widdening ==== 
HFP_TYPE WIDDEN_FRAC = 0.00001; 


// ==== global variable for settings for the objective "APPROACH_ZERO_THEN_HIGH_REL_ERROR_OPT_OBJS" ==== 
time_t AZ_HRE_time_last_global_update;
unsigned int AZ_HRE_sve_last_global_update;
float frac_AZ_to_HRE = (float)1/(float)120; 
float frac_HRE_to_AZ = (float)1/(float)30; 


/* 
   subroutines
*/
void startTimer(time_t *ts) { time(ts); }

bool toAccept (float accept_rate) {
  const unsigned int base = 1000000;
  
  if (((float)(rand() % base) / (float) base) < accept_rate) {
    return true;
  }
  return false;
  
}

bool toRestart (unsigned int &restart_counter) {
  if (toAccept(RRESTART)) {
    restart_counter++;
    return true;
  }
  else return false;
}

void rmFile (const string &fname) {
  stringstream ss; 
#ifdef QUIET_RM
  FILE *temp_open = fopen(fname.c_str(), "r");
  if (temp_open == NULL) return;
  else fclose(temp_open);
#endif 
  ss << "rm " << fname;
  system(ss.str().c_str());
  return; 
}

string CONFtoString (CONF conf) {
  assert(conf.size() > 0);
  stringstream ss;
  assert(conf[0].first == 0);
  ss << 0 << ":" << conf[0].second.index_pars << "/" << conf[0].second.n_pars;
  for (unsigned int vi = 1 ; vi < conf.size() ; vi++) {
    assert(conf[vi].first == vi);
    ss << ", " << vi << ":" << conf[vi].second.index_pars << "/" << conf[vi].second.n_pars;
  }
  return ss.str();
}  

void dumpCONFtoStdout (CONF conf) {
  cout << "---- conf size: " << conf.size() << " ----" << endl;
  for (unsigned int di = 0 ; di < conf.size() ; di++) 
    cout << conf[di].first << " : [" << (CONSOLE_OUTPUT_TYPE) conf[di].second.getLB() << ", " << (CONSOLE_OUTPUT_TYPE) conf[di].second.getUB() << "]" << endl;
  cout << "------------------------------------" << endl;
}


/*
  For the switch rate of some of my experimental objectives 
*/
float switchRateOPTOBJS (ENUM_OPT_OBJS top_objs, ENUM_OPT_OBJS curr_objs) {
  time_t curr_time; 
  
  switch (top_objs) {
  case LAST_HIGHP_VALUE_THEN_LAST_REL_ERROR_OPT_OBJS:
    if (curr_objs == LAST_HIGHP_VALUE_OPT_OBJS) 
      return (RRESTART / 5); 
    else if (curr_objs == LAST_REL_ERROR_OPT_OBJS) 
      return (RRESTART / 10);
    else assert(false && "[ERROR](switchRateOPTOBJS): No such curr_objs"); 
    break;
  case APPROACH_ZERO_THEN_HIGH_REL_ERROR_OPT_OBJS:
    if (curr_objs == LAST_HIGHP_ABSV_OPT_OBJS) {
      if (T_RESOURCE == TIME_TRESOURCE) {
	time(&curr_time); 
	if ((curr_time - AZ_HRE_time_last_global_update) > ((float)TIMEOUT * frac_AZ_to_HRE)) 
	  return 1; 
	else return 0;
      }
      else if (T_RESOURCE == SVE_TRESOURCE) {
	if ((N_RTS - AZ_HRE_sve_last_global_update) > ((float)TIMEOUT * frac_AZ_to_HRE)) 
	  return 1; 
	else return 0;
      }
      else assert(false); 
    }
    else if (curr_objs == LAST_REL_ERROR_OPT_OBJS) {
      if (T_RESOURCE == TIME_TRESOURCE) {
	time(&curr_time); 
	if ((curr_time - AZ_HRE_time_last_global_update) > ((float)TIMEOUT * frac_HRE_to_AZ))
	  return 1; 
	else return 0;
      }
      else if (T_RESOURCE == SVE_TRESOURCE) {
	if ((N_RTS - AZ_HRE_sve_last_global_update) > ((float)TIMEOUT * frac_HRE_to_AZ))
	  return 1; 
	else return 0;
      }
      else assert(false); 
    }
    else assert(false && "[ERROR](switchRateOPTOBJS): No such curr_objs"); 
    break;
  default:
    assert(false && "[ERROR](switchRateOPTOBJS): No such top level objs");
    break;
  }
}

bool switchOPTOBJS (ENUM_OPT_OBJS top_objs, ENUM_OPT_OBJS curr_objs) {
  return toAccept(switchRateOPTOBJS(top_objs, curr_objs));
}


/* 
   Widdening: from a concrete input -> get a CONF 
*/
void widdenFromInput (vector<INPUTV_TYPE> input, const CONF top_conf, CONF &widden) {
  assert(WIDDEN_FRAC <= 1.0);
  assert(WIDDEN_FRAC > 0);
  assert(input.size() == top_conf.size());
  long n_vars = input.size();

  for (long i = 0 ; i < n_vars ; i++) {
    INPUTV_TYPE top_lb = top_conf[i].second.getLB();
    INPUTV_TYPE top_ub = top_conf[i].second.getUB();
    
    if (top_lb > top_ub) top_ub = top_lb; 
   
    INPUTV_TYPE top_gap = top_ub - top_lb; 
    assert(top_gap >= 0);

    INPUTV_TYPE i_lb = input[i] - ((top_gap * WIDDEN_FRAC) / 2.0); 
    i_lb = (i_lb >= top_lb) ? i_lb : top_lb; 

    INPUTV_TYPE i_ub = input[i] + ((top_gap * WIDDEN_FRAC) / 2.0);
    i_ub = (i_ub < top_ub) ? i_ub : top_ub;

    FPR this_fpr (i_lb, i_ub, 0, 1);
    widden.push_back(pair<unsigned int, FPR>(i, this_fpr));
#ifdef S3FP_VERBOSE
    cout << "WIDDEN[" << i << "]: input: " << (CONSOLE_OUTPUT_TYPE) input[i] << " TOP: [" << (CONSOLE_OUTPUT_TYPE) top_lb << ", " << (CONSOLE_OUTPUT_TYPE) top_ub << "]  WIDE: [" << (CONSOLE_OUTPUT_TYPE) i_lb << ", " << (CONSOLE_OUTPUT_TYPE) i_ub << "]" << endl;
#endif 
  }

  assert(widden.size() == n_vars);
}


void LoadInputsFromFile (const char *iname, vector<HFP_TYPE> &ivalues) {
  assert(iname != NULL); 
  FILE *ifile = fopen(iname, "r");
  assert(ifile != NULL);
 
  fseek(ifile, 0, SEEK_END); 
  long fsize = ftell(ifile); 
  fseek(ifile, 0, SEEK_SET);
  assert(fsize % sizeof(HFP_TYPE) == 0);
  fsize = fsize / sizeof(HFP_TYPE); 

  HFP_TYPE idata;   
  for (long ii = 0 ; ii < fsize ; ii++) {
    fread(&idata, sizeof(HFP_TYPE), 1, ifile); 
    ivalues.push_back(idata); 
  }
  
  fclose(ifile); 
}


#endif // #ifndef S3FP_Core
