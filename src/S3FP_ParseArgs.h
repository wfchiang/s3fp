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

#ifndef S3FP_OPT_OBJS
#define S3FP_OPT_OBJS 
enum ENUM_OPT_OBJS {
  NA_OPT_OBJS, 
  // last place rel. error 
  LAST_REL_ERROR_OPT_OBJS, 
  // sum. rel. error 
  SUM_REL_ERROR_OPT_OBJS, 
  // sum. |rel. error|
  ABSSUM_REL_ERROR_OPT_OBJS, 
  // last place ulp error 
  LAST_ULP_ERROR_OPT_OBJS, 
  // last place abs. error 
  LAST_ABS_ERROR_OPT_OBJS, 
  // sum abs. error 
  SUM_ABS_ERROR_OPT_OBJS, 
  // sum. |abs. error| 
  ABSSUM_ABS_ERROR_OPT_OBJS, 
  // last place high value  
  LAST_HIGHP_VALUE_OPT_OBJS, 
  // last place |high value| 
  LAST_HIGHP_ABSV_OPT_OBJS, 
  // sum. high value
  SUM_HIGHP_VALUE_OPT_OBJS, 
  // sum. |high value|
  SUM_HIGHP_ABSV_OPT_OBJS, 
  // last place low value 
  LAST_LOWP_VALUE_OPT_OBJS, 
  // last place |low value|
  LAST_LOWP_ABSV_OPT_OBJS, 
  // sum. low value 
  SUM_LOWP_VALUE_OPT_OBJS, 
  // sum. |low value| 
  SUM_LOWP_ABSV_OPT_OBJS, 

  /*
    Here are some experimental objectives 
  */
  LAST_HIGHP_VALUE_THEN_LAST_REL_ERROR_OPT_OBJS, 
  APPROACH_ZERO_THEN_HIGH_REL_ERROR_OPT_OBJS, 
  DELAUNAY_TRIANGULATION_OPT_OBJS, 
  INC_CONVEX_HULL_OPT_OBJS, 
  DSGESV_OPT_OBJS, 

  OBJECTIVE_AND_REPRESENT_OPT_OBJS, 

  HIGHV_AND_REPRESENT_OPT_OBJS, 
  INNSPHERE_OPT_OBJS, 

  FIRST_FPS_AND_DIS_OPT_OBJS, 
  FPS_AND_DIS_4DIV_OPT_OBJS, 

  GLPK_LP_OPT_OBJS, 
  SORTING_DIV_OPT_OBJS, 
  SCANNING_DIV_OPT_OBJS, 
  KMEANS_OPT_OBJS 
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

/*
#ifndef S3FP_DIV_FUNC 
#define S3FP_DIV_FUNC 
enum ENUM_DIV_FUNC {
  NA_DIV_FUNC, 
  SIGN_DIV_FUNC, 
  FLOOR_DIV_FUNC, 
  CONSIST_DIV_FUNC, 
  GEOMETRIC_DIV_FUNC, 
*/
  /*
    some experimental divergence functions 
  */
/*
  SORTING_DIV_DIV_FUNC, 
  KMEANS_DIV_FUNC, 
  BDECISION_SEQ_DIV_FUNC, 
  INT_SEQ_DIV_FUNC, 
  WEAK_SINGLE_SIG_VIO
}; 
#endif 
*/

#ifndef S3FP_RANDOM_FUNC 
#define S3FP_RANDOM_FUNC
enum ENUM_RANDOM_FUNC {
  NA_RANDOM_FUNC, 
  NAIVE_RANDOM_FUNC
}; 
#endif 

bool testtesttest (); 


string getOPTOBJSString (ENUM_OPT_OBJS opt_objs); 

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
		     unsigned int &n_input_repeats); 
