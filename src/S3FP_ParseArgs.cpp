#include "S3FP_ParseArgs.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <assert.h>


using namespace std;


#define SETTING_FNAME "./s3fp_setting" 
#define SBUFF_SIZE 300


string stringTrim (string ins) {
  int is = 0;
  int ie = ins.length() - 1;
  
  while (ie >= 0) {
    if (ins[ie] == ' ') ie--;
    else break;
  }

  if (ie == 0) return "";
  
  while (is < ie) {
    if (ins[is] == ' ') is++;
    else break; 
  }

  return ins.substr(is, (ie-is)+1);
}


inline 
bool aImpliesB (bool a, bool b) {
  return (a == false || b == true); 
}


bool getNameValue (string sline, string &sname, string &svalue) {
  int ieq = sline.find("=");
  if (ieq == string::npos) return false;
  sname = stringTrim(sline.substr(0, ieq));
  svalue = stringTrim(sline.substr(ieq+1));
  return true;
}


string getOPTREPRESENTString (ENUM_OPT_REPRESENT opt_represent) {
  switch (opt_represent) {
  case UB_OPT_REPRESENT:
    return "UB"; 
    break;
  case LB_OPT_REPRESENT:
    return "LB"; 
    break;
  case MIX_OPT_REPRESENT:
    return "MIX"; 
    break;
  default:
    assert(false && "[ERROR](getOPTREPRESENTString): No such opt. represent");
    break;
  }
}


string getOPTRANGEString (ENUM_OPT_RANGE opt_range) {
  switch (opt_range) {
  case WIDE_OPT_RANGE:
    return "WIDE"; 
    break;
  case NARROW_OPT_RANGE:
    return "NARROW"; 
    break;
  default:
    assert(false && "[ERROR](getOPTRANGEString): No such opt. range"); 
  }
}


ENUM_ERR_OPT stringToErropt (string st) { 
  if (st.compare("FIRST") == 0)
    return FIRST_ERR_OPT; 
  else if (st.compare("LAST") == 0) 
    return LAST_ERR_OPT; 
  else if (st.compare("SUM") == 0) 
    return SUM_ERR_OPT; 
  else if (st.compare("MAX") == 0) 
    return MAX_ERR_OPT; 
  else if (st.compare("AVE") == 0) 
    return AVE_ERR_OPT; 
  else NA_ERR_OPT; 
}


ENUM_ERR_FUNC stringToErrfunc (string st) {
  if (st.compare("REL") == 0) 
    return REL_ERR_FUNC; 
  else if (st.compare("ABSREL") == 0) 
    return ABSREL_ERR_FUNC;
  else if (st.compare("ABSRELMAXONE") == 0)
    return ABSRELMAXONE_ERR_FUNC; 
  else if (st.compare("ABS") == 0)
    return ABS_ERR_FUNC;
  else if (st.compare("ABSABS") == 0) 
    return ABSABS_ERR_FUNC;
  else if (st.compare("LOWP") == 0) 
    return LOWP_ERR_FUNC; 
  else return NA_ERR_FUNC; 
}


ENUM_SIG_FUNC stringToSigfunc (string st) {
  if (st.compare("SINGLE_VAL") == 0) 
    return SINGLE_VAL_SIG_FUNC;
  else if (st.compare("SINGLE_INT") == 0) 
    return SINGLE_INT_SIG_FUNC; 
  else if (st.compare("SINGLE_UINT") == 0) 
    return SINGLE_UINT_SIG_FUNC; 
  else if (st.compare("LAST_VAL") == 0) 
    return LAST_VAL_SIG_FUNC; 
  else if (st.compare("LAST_INT") == 0) 
    return LAST_INT_SIG_FUNC; 
  else if (st.compare("LAST_UINT") == 0) 
    return LAST_UINT_SIG_FUNC; 
  else if (st.compare("BOUNDED_LSE") == 0) 
    return BOUNDED_LSE_SIG_FUNC; 
  else if (st.compare("HIGH_IMPLIES_LOW") == 0) 
    return HIGH_IMPLIES_LOW_SIG_FUNC;  
  else if (st.compare("FIVESTAGE") == 0) 
    return FIVESTAGE_SIG_FUNC; 
  else return NA_SIG_FUNC; 
}


bool S3FP_ParseArgs (unsigned int &n_vars, 
		     string &input_file_name, 
		     ENUM_RT_MODE &rt_mode, 
		     unsigned int &timeout, 
		     ENUM_TRESOURCE &t_resource, 
		     string &exe_lp_name, 
		     string &output_lp_name, 
		     string &exe_hp_name ,
		     string &output_hp_name, 
		     unsigned int &rseed, 
		     ENUM_ERR_OPT &err_opt, 
		     ENUM_ERR_FUNC &err_func, 
		     ENUM_SIG_FUNC &sig_func, 
		     ENUM_SIG_FUNC &diff_con, 
		     ENUM_SIG_FUNC &ext_con, 
		     ENUM_OPT_REPRESENT &opt_represent, 
		     ENUM_OPT_RANGE &opt_range, 
		     HFP_TYPE &rel_delta,
		     bool &uniform_input, 
		     HFP_TYPE &uniform_inputlb, 
		     HFP_TYPE &uniform_inputub, 
		     string &input_range_file, 
		     bool &dump_conf_to_errors, 
		     string &conf_to_errors_file, 
		     ENUM_RANDOM_FUNC &random_func, 
		     bool &check_unstable_error, 
		     bool &awbs_fixed_initialA, 
		     bool &awbs_fivestage_assist, 
		     string &unstable_error_report, 
		     unsigned int &n_input_repeats,
		     bool &backup_input_to_errors, 
		     string &input_to_errors_file, 
		     bool &dump_errors_stream, 
		     string &errors_stream_file, 
		     string &emtest_base_input_file, 
		     string &emtest_pert_type, 
		     HFP_TYPE &emtest_max_pert, 
		     ENUM_ERR_OPT &emtest_in_err_opt, 
		     ENUM_ERR_FUNC &emtest_in_err_func, 
		     ENUM_ERR_OPT &emtest_out_err_opt, 
		     ENUM_ERR_FUNC &emtest_out_err_func, 
		     unsigned int &emtest_n_rounds) {

  bool has_n_vars = false; 
  bool has_input_file_name = false; 
  bool has_rt_mode = false;
  bool has_timeout = false;
  bool has_t_resource = false;
  bool has_exe_lp_name = false; 
  bool has_output_lp_name = false; 
  bool has_exe_hp_name = false; 
  bool has_output_hp_name = false; 
  bool has_rseed = false;
  bool has_err_opt = false; 
  bool has_err_func = false; 
  bool has_sig_func = false; 
  bool has_diff_con = false; 
  bool has_ext_con = false; 
  bool has_opt_represent = false;
  bool has_opt_range = false;
  bool has_uniform_input = false;
  bool has_uniform_inputlb = false;
  bool has_uniform_inputub = false;
  bool has_input_range_file = false;
  bool has_conf_to_errors_file = false;
  bool has_rel_delta = false;
  bool has_random_func = false; 
  bool has_check_unstable_error = false;
  bool has_opt_func = false; 
  bool has_awbs_fixed_initialA = true; 
  bool has_awbs_fivestage_assist = true; 
  bool has_unstable_error_report = false;
  bool has_n_input_repeats = false;
  bool has_backup_input_to_errors = false;
  bool has_dump_errors_stream = false;
  bool has_emtest_base_input_file = false; 
  bool has_emtest_pert_type = false; 
  bool has_emtest_max_pert = false; 
  bool has_emtest_in_err_opt = false; 
  bool has_emtest_in_err_func = false; 
  bool has_emtest_out_err_opt = false; 
  bool has_emtest_out_err_func = false; 
  bool has_emtest_n_rounds = false; 

  ifstream sfile (SETTING_FNAME); 

  char sbuff[SBUFF_SIZE];
  string sname;
  string svalue; 
  while (!sfile.eof()) {
    sfile.getline(sbuff, SBUFF_SIZE); 
    string sline (sbuff);
    getNameValue(sline, sname, svalue);

    if (sname.compare("N_VARS") == 0) {
      n_vars = atoi(svalue.c_str()); 
      has_n_vars = true; 
    }
    else if (sname.compare("INPUT_FILE") == 0) {
      input_file_name = svalue; 
      has_input_file_name = true; 
    }
    else if (sname.compare("RT") == 0) {
      if (svalue.compare("URT") == 0) rt_mode = URT_RT_MODE;
      else if (svalue.compare("BGRT") == 0) rt_mode = BGRT_RT_MODE;
      else if (svalue.compare("ILS") == 0) rt_mode = ILS_RT_MODE;
      else if (svalue.compare("PSO") == 0) rt_mode = PSO_RT_MODE;
      else if (svalue.compare("BURT") == 0) rt_mode = BURT_RT_MODE; 
      else if (svalue.compare("OPT4J") == 0) rt_mode = OPT4J_MODE; 
      else if (svalue.compare("ROBUST_EST") == 0) rt_mode = ROBUST_EST_RT_MODE; 
      else if (svalue.compare("UROBUST_CHECK") == 0) rt_mode = UROBUST_CHECK_RT_MODE; 
      else if (svalue.compare("ABS") || svalue.compare("AWBS") == 0) rt_mode = AWBS_RT_MODE; 
      else if (svalue.compare("EMT") == 0) rt_mode = EMT_RT_MODE; 
      else assert(false);
      has_rt_mode = true;
    }
    else if (sname.compare("TIMEOUT") == 0) {
      timeout = atoi(svalue.c_str());
      has_timeout = true;
    }
    else if (sname.compare("RESOURCE") == 0) {
      if (svalue.compare("TIME") == 0) t_resource = TIME_TRESOURCE;
      else if (svalue.compare("SVE") == 0) t_resource = SVE_TRESOURCE;
      else assert(false);
      has_t_resource = true;
    }
    else if (sname.compare("EXE_LP") == 0) {
      exe_lp_name = svalue; 
      has_exe_lp_name = true; 
    }
    else if (sname.compare("OUTPUT_LP") == 0) {
      output_lp_name = svalue; 
      has_output_lp_name = true; 
    }
    else if (sname.compare("EXE_HP") == 0) {
      exe_hp_name = svalue; 
      has_exe_hp_name = true; 
    }
    else if (sname.compare("OUTPUT_HP") == 0) {
      output_hp_name = svalue; 
      has_output_hp_name = true; 
    }
    else if (sname.compare("RSEED") == 0) {
      rseed = atoi(svalue.c_str());
      has_rseed = true;
    }
    else if (sname.compare("ERR_OPT") == 0) {
      err_opt = stringToErropt(svalue); 
      assert(err_opt != NA_ERR_OPT); 
      has_err_opt = true; 
    }
    else if (sname.compare("ERR_FUNC") == 0) {
      err_func = stringToErrfunc(svalue); 
      assert(err_func != NA_ERR_FUNC); 
      has_err_func = true; 
    }
    else if (sname.compare("SIG_FUNC") == 0) {
      sig_func = stringToSigfunc(svalue); 
      has_sig_func = true; 
    }
    // else if (sname.compare("DIFF_CON") == 0) {
    else if (sname.compare("DIV_FUNC") == 0) {
      diff_con = stringToSigfunc(svalue); 
      has_diff_con = true; 
    }
    else if (sname.compare("EXT_CON") == 0) {
      ext_con = stringToSigfunc(svalue); 
      has_ext_con = true; 
    }
    else if (sname.compare("OPT_REPRESENT") == 0) {
      if (svalue.compare("UB") == 0 || 
	  svalue.compare("ub") == 0) {
	opt_represent = UB_OPT_REPRESENT;
	has_opt_represent = true;
      }
      else if (svalue.compare("LB") == 0 || 
	       svalue.compare("lb") == 0) {
	opt_represent = LB_OPT_REPRESENT;
	has_opt_represent = true;
      }
    }
    else if (sname.compare("OPT_RANGE") == 0) {
      if (svalue.compare("NARROW") == 0 || 
	  svalue.compare("narrow") == 0) {
	opt_range = NARROW_OPT_RANGE;
	has_opt_range = true;
      }
    }
    else if (sname.compare("REL_DELTA") == 0) {
      rel_delta = (HFP_TYPE) atof(svalue.c_str());
      has_rel_delta = true;
    }
    else if (sname.compare("UNIFORM_INPUT") == 0) {
      if (svalue.compare("true") == 0) uniform_input = true;
      else uniform_input = false;
      has_uniform_input = true;
    }
    else if (sname.compare("UNIFORM_INPUTLB") == 0) {
      uniform_inputlb = (HFP_TYPE) atof(svalue.c_str());
      has_uniform_inputlb = true;
    }
    else if (sname.compare("UNIFORM_INPUTUB") == 0) {
      uniform_inputub = (HFP_TYPE) atof(svalue.c_str());
      has_uniform_inputub = true;
    }
    else if (sname.compare("INPUT_RANGE_FILE") == 0) {
      input_range_file = svalue; 
      has_input_range_file = true;
    }
    else if (sname.compare("CONF_TO_ERRORS_FILE") == 0) {
      conf_to_errors_file = svalue;
      has_conf_to_errors_file = true;
      dump_conf_to_errors = true;
    }
    else if (sname.compare("RANDOM_FUNC") == 0) {
      if (svalue.compare("NAIVE") == 0) random_func = NAIVE_RANDOM_FUNC; 
      else random_func = NAIVE_RANDOM_FUNC;
      has_random_func = true; 
    }
    else if (sname.compare("CHECK_UNSTABLE_ERROR") == 0) {
      if (svalue.compare("true") == 0) check_unstable_error = true;
      else check_unstable_error = false;
      has_check_unstable_error = true;
    }
    else if (sname.compare("AWBS_FIXED_INITIALA") == 0) {
      if (svalue.compare("true") == 0) awbs_fixed_initialA = true; 
      else if (svalue.compare("false") == 0) awbs_fixed_initialA = false; 
      else assert(false);
      has_awbs_fixed_initialA = true; 
    }
    else if (sname.compare("AWBS_FIVESTAGE_ASSIST") == 0) {
      if (svalue.compare("true") == 0) awbs_fivestage_assist = true; 
      else if (svalue.compare("false") == 0) awbs_fivestage_assist = false; 
      else assert(false); 
      has_awbs_fivestage_assist = true; 
    }
    else if (sname.compare("UNSTABLE_ERROR_REPORT") == 0) {
      unstable_error_report = svalue; 
      has_unstable_error_report = true; 
    }
    else if (sname.compare("N_INPUT_REPEATS") == 0) {
      n_input_repeats = atoi(svalue.c_str());
      has_n_input_repeats = true;
    }
    else if (sname.compare("INPUT_TO_ERRORS_FILE") == 0) {
      input_to_errors_file = svalue; 
      has_backup_input_to_errors = true;
      backup_input_to_errors = true;
    }
    else if (sname.compare("ERRORS_STREAM_FILE") == 0) {
      errors_stream_file = svalue;
      has_dump_errors_stream = true;
      dump_errors_stream = true;
    }
    else if (sname.compare("EMT_BIFILE") == 0) {
      emtest_base_input_file = svalue; 
      has_emtest_base_input_file = true; 
    }
    else if (sname.compare("EMT_PTYPE") == 0) {
      emtest_pert_type = svalue; 
      has_emtest_pert_type = true; 
    }
    else if (sname.compare("EMT_MAXP") == 0) {
      emtest_max_pert = (HFP_TYPE) atof(svalue.c_str()); 
      has_emtest_max_pert = true; 
    }
    else if (sname.compare("EMT_IN_ERR_OPT") == 0) {
      emtest_in_err_opt = stringToErropt(svalue); 
      assert(emtest_in_err_opt != NA_ERR_OPT); 
      has_emtest_in_err_opt = true; 
    }
    else if (sname.compare("EMT_IN_ERR_FUNC") == 0) {
      emtest_in_err_func = stringToErrfunc(svalue); 
      assert(emtest_in_err_func != NA_ERR_FUNC); 
      has_emtest_in_err_func = true; 
    }
    else if (sname.compare("EMT_OUT_ERR_OPT") == 0) {
      emtest_out_err_opt = stringToErropt(svalue); 
      assert(emtest_out_err_opt != NA_ERR_OPT); 
      has_emtest_out_err_opt = true; 
    }
    else if (sname.compare("EMT_OUT_ERR_FUNC") == 0) {
      emtest_out_err_func = stringToErrfunc(svalue); 
      assert(emtest_out_err_func != NA_ERR_FUNC); 
      has_emtest_out_err_func = true; 
    }
    else if (sname.compare("EMT_N_ROUNDS") == 0) {
      emtest_n_rounds = atoi(svalue.c_str()); 
      has_emtest_n_rounds;
    }
    else {
      cerr << "[ERROR] Unknown setting name: " << sname << endl;
      assert(false); 
    }
  }

  sfile.close();


  if (has_n_vars == false) {
    cout << "[ERROR] # of variables was not specified..." << endl;
    assert(has_n_vars); 
  }
  if (has_input_file_name == false) {
    cout << "[ERROR] Input file name was not specified..." << endl;
    assert(has_input_file_name); 
  }
  if (has_rel_delta == false) {
    cout << "[WARNING] Missing Specification of REL_DELTA" << endl;
    cout << "Assuming REL_DELTA = 0" << endl;
    rel_delta = 0;
  }
  if (has_rt_mode == false) {
    cout << "[ERROR] Missing Specification of RT Mode" << endl;    
    assert(has_rt_mode);
  }
  if (has_timeout == false) {
    cout << "[ERROR] Missing Specification of Timeout" << endl;    
    assert(has_timeout);
  }
  if (has_t_resource == false) {
    cout << "[ERROR] Missing Specification of Resource Type" << endl;    
    assert(has_t_resource);
  }
  if (has_exe_lp_name == false) {
    cout << "[ERROR] Low-precision executable was not specified..." << endl;
    assert(has_exe_lp_name); 
  }
  if (has_output_lp_name == false) {
    cout << "[ERROR] Low-precision output was not specified..." << endl;
    assert(has_output_lp_name); 
  }
  if (has_exe_hp_name == false) {
    cout << "[ERROR] High-precision executable was not specified..." << endl;
    assert(has_exe_hp_name); 
  }
  if (has_output_hp_name == false) {
    cout << "[ERROR] High-precision output was not specified..." << endl;
    assert(has_output_hp_name); 
  }
  // assert(has_rseed);
  if (has_err_opt == false || 
      err_opt == NA_ERR_OPT) {
    cout << "[ERROR] Missing Error Optimization" << endl;
    assert(has_err_opt); 
  }
  if (has_err_func == false || 
      err_func == NA_ERR_FUNC) {
    cout << "[ERROR] Missing Error Function" << endl; 
    assert(has_err_func); 
  }
  if (has_uniform_input == false) {
    cout << "[ERROR] Missing Specification of Using Uniform Input (boolean)" << endl;    
    assert(has_uniform_input);  
  }
  if (!((has_emtest_base_input_file && has_emtest_pert_type && has_emtest_max_pert && has_emtest_in_err_opt && has_emtest_in_err_func && has_emtest_out_err_opt && has_emtest_out_err_func) || 
	(!has_emtest_base_input_file && !has_emtest_pert_type && !has_emtest_max_pert && !has_emtest_in_err_opt && !has_emtest_in_err_func && !has_emtest_out_err_opt && !has_emtest_out_err_func))) {
    cout << "[ERROR] Invalid setting of Error Magnification Test" << endl;
    assert(((has_emtest_base_input_file && has_emtest_pert_type && has_emtest_max_pert && has_emtest_in_err_opt && has_emtest_in_err_func && has_emtest_out_err_opt && has_emtest_out_err_func) || 
	    (!has_emtest_base_input_file && !has_emtest_pert_type && !has_emtest_max_pert && !has_emtest_in_err_opt && !has_emtest_in_err_func && !has_emtest_out_err_opt && !has_emtest_out_err_func)));
  }
  if (!(!has_emtest_base_input_file || 
	emtest_base_input_file.compare(input_file_name) != 0)) {
    cout << "[ERROR] Cannot assigne EMT_BIFILE and INPUT_FILE as the same (" << emtest_base_input_file << " vs " << input_file_name << ")" << endl;
    assert(!has_emtest_base_input_file || 
	   emtest_base_input_file.compare(input_file_name) != 0); 
  }
  if (aImpliesB(uniform_input, has_uniform_inputlb) == false) {
    cout << "[ERROR] Missing Specification of Uniform Lower Bound" << endl;    
    assert(aImpliesB(uniform_input, has_uniform_inputlb));
  }
  if (aImpliesB(uniform_input, has_uniform_inputub) == false) {
    cout << "[ERROR] Missing Specification of Uniform Upper Bound" << endl;    
    assert(aImpliesB(uniform_input, has_uniform_inputub));
  }
  if (aImpliesB(uniform_input==false, has_input_range_file) == false) {
    cout << "[ERROR] Missing Specification of Input Range File" << endl;    
    assert(aImpliesB(uniform_input==false, has_input_range_file));
  }
  if (has_conf_to_errors_file == false) 
    dump_conf_to_errors = false;
  if (has_backup_input_to_errors == false) 
    backup_input_to_errors = false;

  if (rt_mode == NA_RT_MODE) { 
    cout << "[ERROR] Bad Specification of RT Mode" << endl;
    assert(rt_mode != NA_RT_MODE);
  }  
  if (timeout <= 0) {

    cout << "[ERROR] Bad Specification of Timeout" << endl;
    assert(timeout > 0);
  }
  if (t_resource == NA_TRESOURCE) {
    cout << "[ERROR] Bad Specification of Resource Type" << endl;
    assert(t_resource != NA_TRESOURCE);
  }
  if (has_n_input_repeats && 
      n_input_repeats < 1) {
    cout << "[ERROR] Bad Specification of N_INPUT_REPEATS" << endl;
    assert(n_input_repeats >= 1);
  }
  if (check_unstable_error) {
    assert(has_sig_func); 
    assert(has_diff_con); 
    assert(has_unstable_error_report);
  }
  if (has_diff_con && (!has_ext_con)) {
    ext_con = diff_con; 
  }


  return true;
}


