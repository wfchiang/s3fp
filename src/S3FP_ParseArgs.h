#include<iostream>
#include"definitions.h"

using namespace std;

#ifndef S3FP_RT_MODE 
#define S3FP_RT_MODE
enum ENUM_RT_MODE {
  NA_RT_MODE, 
  URT_RT_MODE, 
  BGRT_RT_MODE, 
  ILS_RT_MODE, 
  PSO_RT_MODE, 
  AWBS_RT_MODE, 
  BURT_RT_MODE, 
  OPT4J_MODE, 
  // error magnification test 
  EMT_RT_MODE, 
  // robustness estimation 
  ROBUST_EST_RT_MODE, 
  UROBUST_CHECK_RT_MODE, 
}; 
#endif 

#ifndef S3FP_TRESOURCE 
#define S3FP_TRESOURCE
enum ENUM_TRESOURCE {
  NA_TRESOURCE,
  TIME_TRESOURCE, 
  SVE_TRESOURCE
};
#endif 

#ifndef S3FP_OPT_RANGE 
#define S3FP_OPT_RANGE 
enum ENUM_OPT_RANGE {
  WIDE_OPT_RANGE, 
  NARROW_OPT_RANGE
}; 
#endif 

#ifndef S3FP_OPT_REPRESENT
#define S3FP_OPT_REPRESENT 
enum ENUM_OPT_REPRESENT {
  UB_OPT_REPRESENT, 
  LB_OPT_REPRESENT, 
  MIX_OPT_REPRESENT
}; 
#endif 

#ifndef S3FP_UB_OR_LB
#define S3FP_UB_OR_LB 
enum ENUM_UB_OR_LB {
  ID_UB, 
  ID_LB
}; 
#endif 

/*
#ifndef S3FP_OPT_FUNC 
#define S3FP_OPT_FUNC 
enum ENUM_OPT_FUNC {
  NA_OPT_FUNC, 
  REL_OPT_FUNC, 
  ABS_OPT_FUNC, 
  LOWP_VALUE_OPT_FUNC
};  
#endif 
*/

#ifndef S3FP_ERR_OPT
#define S3FP_ERR_OPT
enum ENUM_ERR_OPT {
  NA_ERR_OPT, 
  FIRST_ERR_OPT, 
  LAST_ERR_OPT, 
  SUM_ERR_OPT, 
  MAX_ERR_OPT, 
  AVE_ERR_OPT 
}; 
#endif 

#ifndef S3FP_ERR_FUNC 
#define S3FP_ERR_FUNC 
enum ENUM_ERR_FUNC {
  NA_ERR_FUNC, 
  REL_ERR_FUNC, 
  ABSREL_ERR_FUNC, 
  ABSRELMAXONE_ERR_FUNC, 
  ABS_ERR_FUNC, 
  ABSABS_ERR_FUNC, 
  LOWP_ERR_FUNC
}; 
#endif 

#ifndef S3FP_SIG_FUNC 
#define S3FP_SIG_FUNC 
enum ENUM_SIG_FUNC {
  NA_SIG_FUNC, 
  SINGLE_VAL_SIG_FUNC, 
  SINGLE_INT_SIG_FUNC, 
  SINGLE_UINT_SIG_FUNC, 
  LAST_VAL_SIG_FUNC, 
  LAST_INT_SIG_FUNC, 
  LAST_UINT_SIG_FUNC, 
  BOUNDED_LSE_SIG_FUNC, 
  HIGH_IMPLIES_LOW_SIG_FUNC, 
  FIVESTAGE_SIG_FUNC
}; 
#endif 

#ifndef S3FP_RANDOM_FUNC 
#define S3FP_RANDOM_FUNC
enum ENUM_RANDOM_FUNC {
  NA_RANDOM_FUNC, 
  NAIVE_RANDOM_FUNC
}; 
#endif 


string getOPTREPRESENTString (ENUM_OPT_REPRESENT opt_represent); 

string getOPTRANGEString (ENUM_OPT_RANGE opt_range);


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
		     ENUM_OPT_REPRESENT &opt_represent, 
		     ENUM_OPT_RANGE &opt_range, 
		     HFP_TYPE &rel_delta,
		     bool &uniform_input, 
		     HFP_TYPE &uniform_inputlb, 
		     HFP_TYPE &uniform_inputub, 
		     string &input_range_file, 
		     ENUM_RANDOM_FUNC &random_func, 
		     bool &check_unstable_error, 
		     bool &awbs_fixed_initialA, 
		     bool &awbs_fivestage_assist, 
		     string &unstable_error_report, 
		     unsigned int &n_input_repeats, 
		     bool &backdoor_use_external_input); 
