#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fstream>
#include <sstream> 
#include <iostream>
#include <vector>
extern "C" {
#include <quadmath.h>
} 

using namespace std;
#define BUFFSIZE 4096
#define S3FP_SETTING "s3fp_setting"
#define RSEED_DELTA 1000 
#define ERRORS_STREAM "error_stream"
#define UNSTABLE_REPORT "unstable_report" 
#define BACKUP_UNSTABLE_INPUT_PREFIX "backup_unstable_input_"

bool record_error_stream = false; 
bool record_unstable_error = true; 


// check if the path exists or not ? 
bool checkPath (string path) {
  ifstream fchecker(path.c_str()); 
  if (fchecker.good()) {
    fchecker.close(); 
    return true;
  }
  else {
    // cerr << "File " << path << " does not exist" << endl;
    fchecker.close(); 
    return false; 
  }
}


// -- main -- 
int main (int argc, char *argv[]) {
  char in_buff[BUFFSIZE];

  // command for executing s3fp 
  string s3fp_command; 
  assert(argc == 1 || argc == 9); 
  if (argc == 1) {
    cout << "[s3fp command]: "; 
    cin.getline(in_buff, BUFFSIZE); 
    string in_s3fp_command (in_buff);
    s3fp_command = in_s3fp_command; 
  }
  else if (argc == 9) {
    string in_s3fp_command; 
    in_s3fp_command.append(argv[1]); 
    in_s3fp_command.append(" "); 
    in_s3fp_command.append(argv[2]); 
    in_s3fp_command.append(" "); 
    in_s3fp_command.append(argv[3]); 
    in_s3fp_command.append(" "); 
    in_s3fp_command.append(argv[4]); 
    in_s3fp_command.append(" "); 
    in_s3fp_command.append("\"");
    in_s3fp_command.append(argv[5]); 
    in_s3fp_command.append("\"");
    in_s3fp_command.append(" "); 
    in_s3fp_command.append(argv[6]); 
    in_s3fp_command.append(" "); 
    in_s3fp_command.append("\"");
    in_s3fp_command.append(argv[7]); 
    in_s3fp_command.append("\"");
    in_s3fp_command.append(" "); 
    in_s3fp_command.append(argv[8]); 
    
    s3fp_command = in_s3fp_command; 
  }
  else assert(false); 

  // -- s3fp setting --
  // RT 
  vector<string> RTs; 
  RTs.push_back("URT");
  RTs.push_back("BGRT"); 
  // TIMEOUT 
  vector<string> TIMEOUTs; 
  TIMEOUTs.push_back("1800"); 
  // RESOURCE 
  string RESOURCE = "TIME"; 
  // FTERROR
  vector<string> FTERRORs; 
  FTERRORs.push_back("FIRST_FPS_AND_DIS"); 
  // OPT_REPRESENT 
  string OPT_REPRESENT = "MIX"; 
  // OPT_RANGE 
  string OPT_RANGE = "WIDE"; 
  // REL_DELTA 
  string REL_DELTA = "0.0"; 
  // PARALLELRT 
  string PARALLELRT = "false"; 
  assert(PARALLELRT.compare("true") == 0 || 
	 PARALLELRT.compare("false") == 0);
  // UNIFORM_INPUT
  string UNIFORM_INPUT = "true"; 
  assert(UNIFORM_INPUT.compare("true") == 0 || 
	 UNIFORM_INPUT.compare("false") == 0); 
  // UNIFORM_INPUTLB 
  string UNIFORM_INPUTLB = "-100.0";
  // UNIFORM_INPUTUB 
  string UNIFORM_INPUTUB = "100.0"; 
  // INPUT_RANGE_FILE 
  string INPUT_RANGE_FILE = "ranges_25_points"; 
  // CHECK_UNSTABLE_ERROR
  string CHECK_UNSTABLE_ERROR = "true"; 
  assert(CHECK_UNSTABLE_ERROR.compare("true") == 0 ||
	 CHECK_UNSTABLE_ERROR.compare("false") == 0); 
  // divergence detection function 
  string DIV_FUNC = "CONSIST"; 
  // # tests 
  unsigned int n_tests = 10; 

  // -- test -- 
  for (vector<string>::iterator rti = RTs.begin() ; 
       rti != RTs.end() ; 
       rti++) {
    for (vector<string>::iterator timeouti = TIMEOUTs.begin() ; 
	 timeouti != TIMEOUTs.end() ; 
	 timeouti++) {
      for (vector<string>::iterator fterrori = FTERRORs.begin() ; 
	   fterrori != FTERRORs.end() ; 
	   fterrori++) {

	// -- stochastic numbers -- 
	// for error stream 
	unsigned long n_errors = 0; 
	__float128 esum = 0.0; 
	__float128 esum2 = 0.0; 
	__float128 emax = 0.0; 
	__float128 emin = 0.0; 
	__float128 epzero = -1.0; 
	__float128 enzero = 1.0; 	
	// for unstable error 
	unsigned int n_tries = 0; 
	int n_detects = 0; 
	long consumed_resource = 0; 
	long var_c_resource = 0.0; 
	vector<long> log_c_resource; 

	for (unsigned int testi = 0 ; testi < n_tests ; testi++) {
	  // -- generate s3fp_setting --
	  ofstream s3fp_sfile (S3FP_SETTING); 

	  s3fp_sfile << "RT = " << (*rti) << endl;

	  s3fp_sfile << "TIMEOUT = " << (*timeouti) << endl;

	  s3fp_sfile << "RESOURCE = " << RESOURCE << endl;

	  s3fp_sfile << "FTERROR = " << (*fterrori) << endl;

	  s3fp_sfile << "RSEED = " << (testi * RSEED_DELTA)  << endl;

	  s3fp_sfile << "OPT_REPRESENT = " << OPT_REPRESENT << endl;

	  s3fp_sfile << "OPT_RANGE = " << OPT_RANGE << endl;

	  s3fp_sfile << "REL_DELTA = " << REL_DELTA << endl;

	  s3fp_sfile << "PARALLELRT = " << PARALLELRT << endl;

	  s3fp_sfile << "UNIFORM_INPUT = " << UNIFORM_INPUT << endl;
	  if (UNIFORM_INPUT.compare("true") == 0) {
	    s3fp_sfile << "UNIFORM_INPUTLB = " << UNIFORM_INPUTLB << endl;
	    s3fp_sfile << "UNIFORM_INPUTUB = " << UNIFORM_INPUTUB << endl; 
	  }
	  else if (UNIFORM_INPUT.compare("false") == 0) {
	    s3fp_sfile << "INPUT_RANGE_FILE = " << INPUT_RANGE_FILE << endl;
	  }
	  else assert(false); 

	  if (record_error_stream)
	    s3fp_sfile << "ERRORS_STREAM_FILE = " << ERRORS_STREAM << endl;
	  
	  if (record_unstable_error)
	    s3fp_sfile << "CHECK_UNSTABLE_ERROR = " << CHECK_UNSTABLE_ERROR << endl;
	  if (CHECK_UNSTABLE_ERROR.compare("true") == 0) {
	    s3fp_sfile << "UNSTABLE_ERROR_REPORT = " << UNSTABLE_REPORT << endl;
	  }
	  else if (CHECK_UNSTABLE_ERROR.compare("false") == 0) ; 
	  else assert(false); 
	  
	  s3fp_sfile << "DIV_FUNC = " << DIV_FUNC << endl;

	  s3fp_sfile.close(); 

	  // -- remove the unstable report --
	  if (checkPath(UNSTABLE_REPORT)) {
	    stringstream ss; 
	    ss << "rm " << UNSTABLE_REPORT; 
	    system(ss.str().c_str()); 
	  }

	  // -- execute s3fp -- 
	  system(s3fp_command.c_str()); 

	  // -- memorize stochasitc numbers -- 
	  if (record_error_stream) {
	    __float128 edata; 
	    FILE *esfile = fopen(ERRORS_STREAM, "r"); 
	    fseek(esfile, 0, SEEK_END); 
	    int fsize = ftell(esfile); 
	    assert(fsize % sizeof(__float128) == 0); 
	    fsize = fsize / sizeof(__float128); 
	    fseek(esfile, 0, SEEK_SET); 
	    
	    for (unsigned int ei = 0 ; ei < fsize ; ei++) {
	      fread(&edata, sizeof(__float128), 1, esfile); 
	      
	      esum += edata; 
	      esum2 += (edata * edata); 
	      
	      if ((n_errors == 0) || (esum > emax)) emax = esum; 
	      if ((n_errors == 0) || (esum < emin)) emin = esum; 
	      if ((esum > 0) && 
		  ((esum < epzero) || (epzero < 0))) epzero = esum; 
	      if ((esum < 0) && 
		  ((esum > enzero) || (enzero > 0))) enzero = esum; 
	      n_errors++; 
	    }
	    fclose(esfile); 
	  }
	  
	  // -- memorize unstable error report --
	  if (record_unstable_error) {
	    FILE *uefile = fopen(UNSTABLE_REPORT, "r");
	    
	    int detected_or_not; 
	    long c_resource; 
	    
	    fscanf(uefile, "%d %ld\n", &detected_or_not, &c_resource); 
	    cout << "Unstable Report: " << detected_or_not << " : " << c_resource << endl;
	    
	    n_tries++; 
	    n_detects += detected_or_not; 
	    consumed_resource += c_resource; 
	    log_c_resource.push_back(c_resource); 

	    if (detected_or_not) {
	      int id = 0; 
	      while (true) {
		stringstream ss; 
		ss << BACKUP_UNSTABLE_INPUT_PREFIX << id; 
		if (checkPath(ss.str()) == false) {
		  stringstream css; 
		  css << "cp unstable_input " << ss.str(); 
		  system(css.str().c_str());
		  break; 
		}
		else id++; 
	      }
	    }

	    fclose(uefile); 
	  }
	}

	if (record_error_stream) {
	  esum = esum / (__float128)n_errors; 
	  esum2 = esum2 / (__float128)n_errors; 
	  cout << "AVE. error: " << (long double) esum << endl;
	  cout << "VAR. error: " << (long double) (esum2 - (esum * esum)) << endl;
	  cout << "MAX. error: " << (long double) emax << endl;
	  cout << "MIN. error: " << (long double) emin << endl;
	  cout << "Arround Error+: " << (long double) epzero << endl;
	  cout << "Arround Error-: " << (long double) enzero << endl;
	}
	
	if (record_unstable_error) {
	  if (n_detects > 0) {
	    if (consumed_resource % n_detects != 0)
	      consumed_resource += n_detects; 
	    consumed_resource = consumed_resource / n_detects; 
	      
	    for (vector<long>::iterator cri = log_c_resource.begin() ; 
		 cri != log_c_resource.end() ; 
		 cri++) {
	      var_c_resource += (((*cri) - consumed_resource) * ((*cri) - consumed_resource)); 
	    }
	    if (var_c_resource % n_detects != 0)
	      var_c_resource += n_detects; 
	    var_c_resource = var_c_resource / n_detects; 
	  }
	    
	  cout << "# TRIES: " << n_tries << endl;
	  cout << "# DETECTS: " << n_detects << endl;
	  cout << "AVE. RESOURCE in DETECTS: " << consumed_resource << endl;
	  cout << "VAR. RESOURCE in DETECTS: " << var_c_resource << endl;
	}
      }
    }
  }

  return 0;
}
