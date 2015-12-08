#!/usr/bin/env python 

import os 
import sys
import random 

# ========
# global variables 
# ========
RT = None 
N_VARS = None 
INPUT_FILE = None 
EXE_LP = None 
OUTPUT_LP = None 
EXE_HP = None
OUTPUT_HP = None 
TIMEOUT = None 
RESOURCE = None 
RSEED = None 
REL_DELTA = None 
UNIFORM_INPUT = None 
UNIFORM_INPUTLB = None 
UNIFORM_INPUTUB = None 
INPUT_RANGE_FILE = None 
ERR_FUNC = None 
ERR_OPT = None 
SIG_FUNC = None 
DIV_FUNC = None 


# ========
# sub-routines 
# ========
def S3FPBasicSetting (fname): 
    global INPUT_FILE 
    global OUTPUT_LP 
    global OUTPUT_HP 
    global TIMEOUT 
    global RESOURCE 

    INPUT_FILE = "random_input" 
    OUTPUT_LP = "output_lp" 
    OUTPUT_HP = "output_hp" 
    TIMEOUT = int(5000) 
    RESOURCE = "SVE" 

    assert(RT in ["URT", "BGRT", "ILS", "PSO", "ABS"]) 
    assert((type(N_VARS) is int) and (0 < N_VARS))
    assert(type(INPUT_FILE) is str) 
    assert((type(EXE_LP) is str) and (type(OUTPUT_LP) is str)) 
    assert((type(EXE_HP) is str) and (type(OUTPUT_HP) is str)) 
    assert((type(TIMEOUT) is int) and (0 < TIMEOUT)) 
    assert(RESOURCE in ["TIME", "SVE"])
    assert((type(RSEED) is int) and (0 <= RSEED)) 
    assert((type(REL_DELTA) is float) and (0 <= REL_DELTA)) 

    assert(ERR_FUNC in ["REL", "ABSREL", "ABSRELMAXONE", "ABS", "ABSABS", "LOWP"])
    assert(ERR_OPT in ["FIRST", "LAST", "SUM", "MAX", "AVE"]) 

    assert(UNIFORM_INPUT in ["true", "false"])
    if (UNIFORM_INPUT == "true"): 
        assert((type(UNIFORM_INPUTLB) is float) and (type(UNIFORM_INPUTUB) is float) and (UNIFORM_INPUTLB <= UNIFORM_INPUTUB)) 
    else:
        assert(type(INPUT_RANGE_FILE) is str) 
        
    sfile = open(fname, "w") 

    sfile.write("RT = " + RT + "\n") 
    sfile.write("N_VARS = " + str(N_VARS) + "\n") 
    sfile.write("TIMEOUT = " + str(TIMEOUT) + "\n") 
    sfile.write("RESOURCE = " + RESOURCE + "\n") 
    sfile.write("RSEED = " + str(RSEED) + "\n") 
    sfile.write("REL_DELTA = " + str(REL_DELTA) + "\n") 

    sfile.write("ERR_FUNC = " + ERR_FUNC + "\n") 
    sfile.write("ERR_OPT = " + ERR_OPT + "\n") 

    sfile.write("INPUT_FILE = " + INPUT_FILE + "\n") 
    sfile.write("EXE_LP = " + EXE_LP + "\n") 
    sfile.write("OUTPUT_LP = " + OUTPUT_LP + "\n") 
    sfile.write("EXE_HP = " + EXE_HP + "\n") 
    sfile.write("OUTPUT_HP = " + OUTPUT_HP + "\n") 

    sfile.write("UNIFORM_INPUT = " + UNIFORM_INPUT + "\n") 
    if (UNIFORM_INPUT == "true"): 
        sfile.write("UNIFORM_INPUTLB = " + str(UNIFORM_INPUTLB) + "\n") 
        sfile.write("UNIFORM_INPUTUB = " + str(UNIFORM_INPUTUB) + "\n") 
    else: 
        assert(UNIFORM_INPUT == "false") 
        sfile.write("INPUT_RANGE_FILE = " + INPUT_RANGE_FILE + "\n") 

    sfile.close() 


def S3FPDivSetting (fname): 
    assert(SIG_FUNC in ["SINGLE_INT", "SINGLE_UINT", "LAST_INT", "LAST_UINT"]) 
    assert(DIV_FUNC in ["SINGLE_INT", "SINGLE_UINT", "LAST_INT", "LAST_UINT"]) 
    
    sfile = open(fname, "a")

    sfile.write("SIG_FUNC = " + SIG_FUNC + "\n") 
    sfile.write("DIV_FUNC = " + DIV_FUNC + "\n") 
    sfile.write("CHECK_DIV = true\n") 
    sfile.write("DIV_ERROR_REPORT = __div_error\n") 

    sfile.close() 


def SetUniformInput (vlb, vub): 
    global UNIFORM_INPUT 
    global UNIFORM_INPUTLB 
    global UNIFORM_INPUTUB 

    assert((type(vlb) is float) and (type(vub) is float) and (vlb <= vub)) 
    
    UNIFORM_INPUT = "true" 
    UNIFORM_INPUTLB = vlb 
    UNIFORM_INPUTUB = vub 
    

def TestRoundoff (dir_bench, dir_back): 
    os.chdir(dir_bench)

    S3FPBasicSetting("s3fp_setting") 
    
    os.system(S3FP) 

    os.chdir(dir_back) 


def TestDiv (dir_bench, dir_back): 
    os.chdir(dir_bench) 

    S3FPBasicSetting("s3fp_setting") 
    S3FPDivSetting("s3fp_setting") 

    os.system(S3FP) 
    
    os.chdir(dir_back) 
    


# ========
# main 
# ========
assert(len(sys.argv) == 5) 

MODE = sys.argv[1] 
BENCH = sys.argv[2] 
RT = sys.argv[3] 
RSEED = int(sys.argv[4]) 

DIR_CURR = os.getcwd() 
S3FP = DIR_CURR + "/../src/s3fp" 
DIR_BENCH = "" 

assert(MODE in ["round-off", "div"]) 
assert(RT in ["URT", "BGRT", "ILS", "PSO", "ABS"]) 

if (MODE == "round-off"): 
    assert(RT in ["URT", "BGRT", "ILS", "PSO"]) 

    DIR_BENCH = "./tests-fp-error" 

    ERR_FUNC = "REL" 
    REL_DELTA = float(0.001) 

    if (BENCH == "balanced-reduction"): 
        N_VARS = 512 
        EXE_LP = "balanced_reduction_32" 
        EXE_HP = "balanced_reduction_128" 
        ERR_OPT = "LAST"
        
        SetUniformInput(-100.0, 100.0) 

        TestRoundoff(DIR_BENCH+"/balanced-reduction", DIR_CURR)

    elif (BENCH == "imbalanced-reduction"): 
        N_VARS = 512 
        EXE_LP = "imbalanced_reduction_32" 
        EXE_HP = "imbalanced_reduction_128" 
        ERR_OPT = "LAST" 
        
        SetUniformInput(-100.0, 100.0) 

        TestRoundoff(DIR_BENCH+"/imbalanced-reduction", DIR_CURR) 

    else: 
        sys.exit("Error: unknown benchmark for round-off demo : " + BENCH) 


elif (MODE == "div"): 
    assert(RT in ["BGRT", "ABS"]) 

    DIR_BENCH = "./tests-div-detection" 

    ERR_FUNC = "REL" 
    ERR_OPT = "FIRST" 
    REL_DELTA = float(0.0)    

    if (BENCH in ["pp_3x3", "pp_4x4", "pc_3x3", "pc_4x4", "ps_4x4", "ps_5x5"]): 
        if (BENCH in ["pp_3x3", "pp_4x4"]): 
            N_VARS = 12 
        elif (BENCH in ["pc_3x3", "pc_4x4"]): 
            N_VARS = 8 
        elif (BENCH in ["ps_4x4", "ps_5x5"]):
            N_VARS = 15 
        else:
            sys.exit("Error: broken control flow...") 

        EXE_LP = BENCH + "_32" 
        EXE_HP = BENCH + "_128" 
        SIG_FUNC = "LAST_INT"
        DIV_FUNC = "LAST_INT" 

        SetUniformInput(-100.0, 100.0) 

        TestDiv(DIR_BENCH+"/"+BENCH, DIR_CURR) 

    else: 
        sys.exit("Error: unknown benchmark for div. demo : " + BENCH) 

        
else: 
    sys.exit("Error: unknown demo. mode : " + MODE) 
