#!/usr/bin/env python 

import os
import time
import subprocess as sub

OPT4J = "/home/wfchiang/tools/opt4j-3.0.1/bin/opt4j" 
TIMEOUT = 1800 
N_TESTS = 1 
ELAPSES = []
BACKUP_STDOUT = True
BACKUP_STDERR = True
OPT4J_STDOUT_PREFIX = "opt4j_stdout_"
OPT4J_STDERR_PREFIX = "opt4j_stderr_"
PRESERVED_INPUT_PREFIX = "saved_random_input_" 


opt4j_command = OPT4J + " -s ./opt4j-config.xml" 


# ==== subroutines ====
def PreserveInput (): 
    time_random_main = 0
    time_random_pool = 0
    name_random_main = "./random_input-main" 
    name_random_pool = "./random_input-pool-1-thread-1"
    latest_fname = ""
    if (os.path.exists(name_random_main)):
        time_random_main = os.path.getmtime("./random_input-main")
    if (os.path.exists(name_random_pool)):
        time_random_pool = os.path.getmtime("./random_input-pool-1-thread-1") 
    assert(time_random_main != 0 or time_random_pool != 0)
    if (time_random_main > time_random_pool):
        latest_fname = name_random_main
    else:
        latest_fname = name_random_pool 
    iid = 0
    while (True):
        preserved_fname = PRESERVED_INPUT_PREFIX + str(iid)
        if (os.path.exists(preserved_fname) == False):
            os.system("cp " + latest_fname + " " + preserved_fname)
            break
        iid = iid + 1

# a process tells whoami 
def WhoAmI ():
    whoami = sub.Popen("whoami", shell=True, stdout=sub.PIPE, stderr=sub.PIPE)
    for aline in whoami.stdout: 
        return aline.strip()
    return ""

# kill a process family 
def KillOpt4j (whoami, mpid):
    got_mother = False

    psh = sub.Popen("ps -u " + whoami + " -H", shell=True, stdout=sub.PIPE, stderr=sub.PIPE)
    for aline in psh.stdout:
        aline = aline.strip()
        i = aline.find(' ')
        assert(0 < i)
        
        this_pid = -1
        try:
            this_pid = int(aline[0:i])
        except:
            this_pid = -1
            pass 
        if (this_pid == -1):
            continue
        
        if (got_mother == False):
            if (this_pid == mpid):
                got_mother = True
        else:
            if (aline.find("java") > 0):
                os.system("kill -9 " + str(this_pid))
            else:
                os.system("WARNING: Failed to kill opt4j...")
            break
    assert(got_mother)

# ==== main ====

print "Remove All stdout and stderr files" 
os.system("rm " + OPT4J_STDOUT_PREFIX + "*")
os.system("rm " + OPT4J_STDERR_PREFIX + "*")

print "Append LD_LIBRARY_PATH" 
os.environ['LD_LIBRARY_PATH'] = "../../lib/div_geometric_primitives:" + os.environ['LD_LIBRARY_PATH']

for ti in range(0, N_TESTS):
    id_etag = -1 
    test_success = False
    opt4j_run = sub.Popen(opt4j_command, shell=True, stdout=sub.PIPE, stderr=sub.PIPE)

    for tt in range(0, TIMEOUT):
        # sleep first 
        time.sleep(1)
        # check if opt4j terminated 
        if (opt4j_run.poll() is None):
            pass
        else: # opt4j was terminated 
            # backup stdout 
            if (BACKUP_STDOUT): 
                outfile = open(OPT4J_STDOUT_PREFIX + str(ti), "w")
                for aline in opt4j_run.stdout : 
                    outfile.write(aline)
                outfile.close
            # backup stderr 
            if (BACKUP_STDERR):
                errfile = open(OPT4J_STDERR_PREFIX + str(ti), "w")
                for aline in opt4j_run.stderr :
                    id_etag = aline.find("Divergence Detected !!")
                    if (id_etag >= 0): # A divergence was really found 
                        test_success = True
                        ELAPSES.append(tt) 
                        PreserveInput() 
                    errfile.write(aline)
                errfile.close
            # this iteration was finished
            break
    if (test_success == False):
        print "TEST " + str(ti) + " failed..." 
        KillOpt4j(WhoAmI(), opt4j_run.pid)
        opt4j_run.kill()
    else:
        print "TEST " + str(ti) + " successed !! (" + str(tt) + " sec.)" 

# report 
n_success = len(ELAPSES)
print "success rate = " + str(n_success) + "/" + str(N_TESTS) 
if (n_success > 0):
    tsum = 0 
    t2sum = 0
    for tt in ELAPSES:
        tsum = tsum + tt 
        t2sum = t2sum + (tt * tt) 
    tave = float(tsum) / float(n_success)
    tvar = (float(t2sum) / float(n_success)) - (tave * tave)
    print "ave. time of success : " + str(tave)
    print "var. time of success : " + str(tvar)
else:
    print "ave. time of success : N/A" 
    print "var. time of success : N/A" 
    



