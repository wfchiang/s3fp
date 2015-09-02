#!/usr/bin/env python 

import os 
import sys 
import subprocess as subp 

INPUTNAME = "random_input" 
OUTDUMP = "__crc_out" 
CRC_DIR = "" 


# ==== get all testing inputs ==== 
def GetAllTestingInputs (tdir): 
    inputs = [] 

    os.chdir(tdir) 

    cmd_ls = subp.Popen("ls input-awbs-*", shell=True, stdout=subp.PIPE, stderr=subp.PIPE) 
    for aline in cmd_ls.stdout: 
        aline = aline.strip() 
        inputs.append(aline) 

    os.chdir(CRC_DIR)

    return inputs 


# ==== remake a dir. ==== 
def RemakeDir (tdir):
    os.chdir(tdir) 

    os.system("make clean ; make ") 
    
    os.chdir(CRC_DIR)


# ==== check if div. from an output file ==== 
def CheckDivFromOutFile (oname): 
    rel = [] 
    ofile = open(oname, "r") 
    
    for aline in ofile: 
        aline = aline.strip() 
        if (aline.startswith("[1]: ")): 
            rel.append(float(aline[5:])) 
    assert(len(rel) == 2)

    ofile.close() 

    return (not (rel[0] == rel[1])) 


# ==== perform tests ==== 
def CrossTests (tdir, tinputs, cdir): 
    n_divs = 0 

    os.chdir(cdir) 

    for i in tinputs: 
        os.system("cp " + tdir + "/" + i + " " + INPUTNAME)
        os.system("make test > " + OUTDUMP) 
        if (CheckDivFromOutFile(OUTDUMP)): 
            n_divs = n_divs + 1 

    os.chdir(CRC_DIR) 

    return n_divs 


# ==== test round ==== 
def CRCRival (dirA, dirB): 
    print " ==== " + dirA + " vs " + dirB + " ==== " 

    inputsA = GetAllTestingInputs(dirA)
    inputsB = GetAllTestingInputs(dirB)

    assert(len(inputsA) == len(inputsB)) 
    
    scoreA = CrossTests(dirB, inputsB, dirA)
    scoreB = CrossTests(dirA, inputsA, dirB)

    print "player A's score: " + str(scoreA) 
    print "player B's score: " + str(scoreB) 


# ==== main ====
CRC_DIR = os.getcwd() 

RIVAL_PAIRS = [[os.getcwd() + "/pp_3x3", os.getcwd() + "/pp_4x4"], [os.getcwd() + "/pc_3x3", os.getcwd() + "/pc_4x4"], [os.getcwd() + "/ps_4x4", os.getcwd() + "/ps_5x5"]] 

for rp in RIVAL_PAIRS: 
    RemakeDir(rp[0])
    RemakeDir(rp[1])

os.system("./auto_tester.py awbs 1800 TIME 200 -100 100") 

for rp in RIVAL_PAIRS: 
    CRCRival(rp[0], rp[1]) 









