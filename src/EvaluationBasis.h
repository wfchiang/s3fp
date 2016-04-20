#pragma once

#include "definitions.h"
#include "FPR.h"
#include <vector>
#include <map> 

using namespace std;



void ExtractEEstSigValues (vector<HFP_TYPE> poutputs, 
			   vector<HFP_TYPE> &err_est_vals, 
			   vector<HFP_TYPE> &div_det_vals) {
  assert(poutputs.size() >= 1); 
  err_est_vals.clear();
  div_det_vals.clear(); 

  if (poutputs.size() == 1) {
    err_est_vals.push_back(poutputs[0]); 
  }
  /*
  else if (poutputs.size() == 2) {
    err_est_vals.push_back(poutputs[0]);
    div_det_vals.push_back(poutputs[1]); 
  }
  */
  else { 
    HFP_TYPE n_eev = poutputs[0]; 
    assert(n_eev == floorq(n_eev)); 
    assert(n_eev + 1 <= poutputs.size()); 
    assert(n_eev >= 1); 
    
    HFP_TYPE n_ddv = ((poutputs.size() - (n_eev + 1) == 0) ? 
		      0 : 
		      poutputs[1 + n_eev]); 
    assert(n_ddv == floorq(n_ddv)); 
    assert(n_ddv >= 0); 
    if (n_ddv == 0) {
      assert(poutputs.size() == 1 + n_eev); 
    }
    else { // n_ddv > 0 
      assert(poutputs[1 + n_eev] > 0);
      assert(poutputs.size() == 1 + n_eev + 1 + n_ddv); 
    }
    
    for (unsigned int i = 0 ; i < n_eev ; i++) {
      err_est_vals.push_back(poutputs[1 + i]); 
    }
    for (unsigned int i = 0 ; i < n_ddv ; i++) {
      div_det_vals.push_back(poutputs[2 + n_eev + i]); 
    }
  }
}



/**
 * A class which runs external programs
 */
class EvaluationBasis {
 private:
  unsigned int n_inputs; 
  string exeLP;
  string outnameLP;
  string exeHP;
  string outnameHP;
  string inputname;
  unsigned int n_input_repeats; 

  string rtIOName(const string &prefix, unsigned int pid, unsigned int tid) const {
    stringstream rtss;
    rtss << prefix << "." << pid << "." << tid;
    return rtss.str();
  }

  // HFP_TYPE 
  void readOutput(const string &outname, 
		  vector<HFP_TYPE> &ret, 
		  int *error) const {
    FILE *fp = fopen(outname.c_str(), "r");

    if (fp) {
      fseek(fp, 0, SEEK_END);
      long int n_fbytes = ftell(fp);
      fseek(fp, 0, SEEK_SET);
#ifdef S3FP_DDIO
      assert(n_fbytes % (sizeof(double)*2) == 0);
      long int n_foutputs = n_fbytes / (sizeof(double)*2);
#else
      assert(n_fbytes % sizeof(HFP_TYPE) == 0);
      long int n_foutputs = n_fbytes / sizeof(HFP_TYPE);
#endif

      for (unsigned int i = 0 ; i < n_foutputs ; i++) {
	HFP_TYPE val;
	assert(!feof(fp));
#ifdef S3FP_DDIO
	OUTPUTV_TYPE outv; 
	fread(&outv.x[0], sizeof(double), 1, fp);
	fread(&outv.x[1], sizeof(double), 1, fp);

	// comvert from dd_real to HFP_TYPE
	val = (HFP_TYPE) to_double(outv);
#else 
	fread(&val, sizeof(HFP_TYPE), 1, fp);
#endif 
	ret.push_back(val);
      }
      fclose(fp);
      *error = 0;
    }
    else {
      cout << "Output file " << outname << " doesn't exist" << endl;
      *error = 1;
    }
  }

 public:
  EvaluationBasis () {
  }

  EvaluationBasis (unsigned int in_n_inputs, 
		   const string &in_exeLP, 
		   const string &in_outnameLP, 
		   const string &in_exeHP, 
		   const string &in_outnameHP, 
		   const string &in_inputname,
		   unsigned int in_n_input_repeats) {
    assert (in_n_input_repeats >= 1);

    n_inputs = in_n_inputs;
    exeLP = in_exeLP;
    outnameLP = in_outnameLP;
    exeHP = in_exeHP;
    outnameHP = in_outnameHP;
    inputname = in_inputname;
    n_input_repeats = in_n_input_repeats;
  }

  EvaluationBasis &operator = (const EvaluationBasis &rhs) {
    n_inputs = rhs.n_inputs;

    exeLP = rhs.exeLP;
    outnameLP = rhs.outnameLP;
    exeHP = rhs.exeHP;
    outnameHP = rhs.outnameHP;
    inputname = rhs.inputname;
    n_input_repeats = rhs.n_input_repeats;

    return *this;
  }

  string getInputname() const {
    return inputname;
  }

  void runEXE (int *error, 
	       vector<HFP_TYPE> &outputs, 
	       string exename, 
	       string inname, 
	       string outname) const { 
	       
    stringstream exe_para;

    outputs.clear();

    rmFile(outname);
    exe_para << exename << " " << inname << " " << outname; 

    stringstream run;
    run << exe_para.str() << " 2>&1 > __outdump"; 

    system(run.str().c_str());

    // return 
    readOutput(outname, outputs, error);
  }

  // Runs the low precision program on the current input and returns the result
  // HFP_TYPE 
  void runLP (int *error, 
	      vector<HFP_TYPE> &outputs) const { 
    runEXE (error, 
	    outputs, 
	    exeLP, inputname, outnameLP); 
  }

  void runLP (int *error, 
	      vector<HFP_TYPE> &err, 
	      vector<HFP_TYPE> &sig) {
    err.clear(); sig.clear(); 
    vector<HFP_TYPE> output; 
    runLP(error, output); 
    assert(output.size() > 0); 
    ExtractEEstSigValues (output, err, sig); 
  }
	      

  // Runs the high precision program on the current input and returns the result
  // HFP_TYPE 
  void runHP (int *error, 
	      vector<HFP_TYPE> &outputs) const { 
    runEXE (error, 
	    outputs, 
	    exeHP, inputname, outnameHP); 
  }

  void runHP (int *error, 
	      vector<HFP_TYPE> &err, 
	      vector<HFP_TYPE> &sig) {
    err.clear(); sig.clear(); 
    vector<HFP_TYPE> output; 
    runHP(error, output); 
    assert(output.size() > 0); 
    ExtractEEstSigValues (output, err, sig); 
  }


  void prepareInput (const CONF &conf, 
		     ENUM_RANDOM_FUNC rfunc = NAIVE_RANDOM_FUNC, 
		     unsigned int _pid = 0, unsigned int _tid = 0) const {
    const string ifname = inputname; 

    for (unsigned int i = 0 ; i < n_input_repeats ; i++) {
      const char *write_mode = (i == 0) ? "w" : "a";
      sampleCONF(ifname, conf, rfunc, write_mode);
    }
  }

  void prepareInput (vector<INPUTV_TYPE> input, string write_mode) {
    assert(write_mode.compare("w") == 0 || 
	   write_mode.compare("a") == 0); 
    FILE *inputfile = fopen(inputname.c_str(), write_mode.c_str()); 
    assert(inputfile != NULL); 

    for (unsigned int i = 0 ; i < input.size() ; i++) {
      INPUTV_TYPE idata = input[i]; 
      fwrite(&idata, sizeof(INPUTV_TYPE), 1, inputfile); 
    }

    fclose(inputfile); 
  }

  unsigned int getNInputRepeats() { return n_input_repeats; }

  unsigned int getNInputs() { return n_inputs; }

  string getExeLP() { return exeLP; }
  string getExeHP() { return exeHP; }

  void getInputValues (vector<INPUTV_TYPE> &ivs, 
		       unsigned int N_INPUT_REPEATS, 
		       string iname = "") {
    assert(ivs.size() == 0); 
   
    FILE *sfile = fopen(((iname.compare("") == 0) ? 
			 getInputname().c_str() : iname.c_str()), 
			"r"); 
    assert(sfile != NULL); 

    fseek(sfile, 0, SEEK_END); 
    long fsize = ftell(sfile); 
    fseek(sfile, 0, SEEK_SET); 
    assert(fsize == sizeof(INPUTV_TYPE) * n_inputs * N_INPUT_REPEATS); 

    for (int i = 0 ; i < (n_inputs * N_INPUT_REPEATS) ; i++) {
      INPUTV_TYPE idata; 
      fread(&idata, sizeof(INPUTV_TYPE), 1, sfile); 
      ivs.push_back(idata); 
    }
    
    fclose(sfile); 
  }
};
