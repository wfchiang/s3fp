
import re

# ==== variables ==== 

VERBOSE=True

TMETHOD=""
TIMEOUT=0
RESOURCE=""
N_TESTS=0
UNIFORM_LB=1.0
UNIFORM_UB=-1.0 

EXT_TIME_MEASUREMENT = True 
TOTAL_N_SAMPLES = 0

LAST_RSEED = -1
RSEED_GAP = 0
LAST_RSEED_NAME = "__testing_utils_last_rseed" 
RSEED_GAP_NAME = "__testing_utils_rseed_gap"

S3FP_OUTFILE = "s3fp_outdump"


# ==== sub-routines ==== 
def GetLastRSEED (): 
    lastrs = 0
    has_lastrs = False 
    rsfile = open(LAST_RSEED_NAME, "r")
    
    for aline in rsfile: 
        lastrs = int(aline) 
        has_lastrs = True
        break 

    rsfile.close()
    assert(has_lastrs) 
    return lastrs 


def GetRSEEDGAP (): 
    lastrs = 0
    has_lastrs = False 
    rsfile = open(RSEED_GAP_NAME, "r")
    
    for aline in rsfile: 
        lastrs = int(aline) 
        has_lastrs = True
        break 

    rsfile.close()
    assert(has_lastrs) 
    return lastrs 


def PutLastRSEED (lastrs): 
    rsfile = open(LAST_RSEED_NAME, "w") 
    rsfile.write(str(lastrs))
    rsfile.close()


def PutRSEEDGAP (rsgap): 
    rsfile = open(RSEED_GAP_NAME, "w")
    rsfile.write(str(rsgap))
    rsfile.close()


def HandleSpecialUsage (argv = []):
    if (len(argv) == 2): 
        command = argv[1].strip()
        if (command == "reset-rseed"): 
            ResetRSEED() 
            exit()
    if (len(argv) == 3):
        command = argv[1].strip()
        value = int(argv[2].strip())
        if (command == "set-rseed"): 
            PutLastRSEED(value)
            exit()
        if (command == "set-rsgap"):
            PutRSEEDGAP(value)
            exit()


def WrongUsage (desired_argc): 
    if (desired_argc == 7): 
        print "auto_tester?.py [testing-method] [timeout] [resource-type] [#-tests] [uniform-lb] [uniform-ub]" 
    else: 
        print "ERROR: Invalid desired argc"
        assert(False)

    exit()


def SetGlobalVariables (argv = []): 
    global TMETHOD 
    global TIMEOUT 
    global RESOURCE
    global N_TESTS
    global UNIFORM_LB
    global UNIFORM_UB 

    assert(len(argv) >= 7)

    TMETHOD = str(argv[1])
    assert(TMETHOD == "urob" or TMETHOD == "bgrt" or TMETHOD == "opt4j" or TMETHOD == "awbs")

    TIMEOUT = int(argv[2])
    assert(TIMEOUT > 0)

    RESOURCE = str(argv[3])
    assert(RESOURCE == "TIME" or RESOURCE == "SVE") 

    N_TESTS = int(argv[4])
    assert(N_TESTS > 0)

    UNIFORM_LB = float(argv[5])
    UNIFORM_UB = float(argv[6]) 
    assert(UNIFORM_LB < UNIFORM_UB)


def GetRSEEDS (): 
    global LAST_RSEED 

    lastrs = LAST_RSEED

    rseeds = range(lastrs, (lastrs + (N_TESTS*RSEED_GAP)), RSEED_GAP)    
    lastrs = lastrs + (N_TESTS * RSEED_GAP) 

    LAST_RSEED = lastrs 

    return rseeds 


def SetRSEED (rs): 
    PutLastRSEED(rs) 


def SetRSGEP (rg): 
    PutRSEEDGAP(rg)


def ResetRSEED ():
    SetRSEED(0)


def LoadRSEEDSetting (): 
    global LAST_RSEED
    global RSEED_GAP

    LAST_RSEED = GetLastRSEED()
    RSEED_GAP = GetRSEEDGAP() 


def AWBSDivOutcome (fname): 
    i2first = -1
    t2first = -1 
    n_restarts = -1

    ofile = open(fname, "r")

    for aline in ofile: 
        aline = aline.strip()

        if (aline.startswith("Div. Detected after : ") and aline.endswith(" samples")): 
            rer = re.findall(r"\D(\d+)\D", aline)
            assert(len(rer) == 1)
            
            assert(i2first < 0)
            i2first = int(rer[0])
            assert(i2first > 0)
            
        if (aline.startswith("Found a div. in time: ")): 
            rer = re.findall(r"\D(\d+)", aline)
            assert(len(rer) == 1)

            assert(t2first < 0)
            t2first = int(rer[0])
            assert(t2first >= 0)

        if (aline.startswith("# Restarts: ")): 
            rer = re.findall(r"\D(\d+)", aline)
            assert(len(rer) == 1)

            assert(n_restarts < 0) 
            n_restarts = int(rer[0])
            assert(n_restarts >= 0)

        if (aline.startswith("Cannot detect div. within timeout (") and aline.endswith(" samples)")): 
            rer = re.findall(r"\D(\d+)\D", aline) 
            assert(len(rer) == 1) 

            assert(t2first < 0) 
            assert(i2first < 0) 
            t2first = -1 
            i2first = int(rer[0])
            assert(t2first < 0) 
            assert(i2first > 0)
            
    ofile.close()

    return [t2first, i2first, n_restarts] 


def BGRTDivOutcome (fname): 
    i2first = -1
    t2first = -1 
    ivalid = -1

    ofile = open(fname, "r")

    for aline in ofile: 
        aline = aline.strip()

        if (aline.startswith("HALT EARLY: # of SVE : ")): 
            rer = re.findall(r"\D(\d+)", aline)
            assert(len(rer) == 1)

            assert(i2first < 0)
            i2first = int(rer[0])
            assert(i2first > 0)

        if (aline.startswith("HALT EARLY: time : ")): 
            rer = re.findall(r"\D(\d+)", aline)
            assert(len(rer) == 1)

            assert(t2first < 0)
            t2first = int(rer[0])
            assert(t2first >= 0)
            
        if (aline.startswith("N VALID BGRT TESTS: ")): 
            rer = re.findall(r"\D(\d+)", aline)
            assert(len(rer) == 1)

            assert(ivalid < 0)
            ivalid = int(rer[0])
            assert(ivalid >= 0)
            
    ofile.close()

    if (t2first >= 0): 
        return [t2first, i2first]
    else: 
        return [-1, ivalid]


def UROBDivOutcome (fname): 
    ndivs = 0 
    nsamples = 0

    ofile = open(fname, "r")

    for aline in ofile: 
        aline = aline.strip()

        if (aline.startswith("# Divs: ")):
	    rer = re.findall(r"\D(\d+)", aline)
            assert(len(rer) == 1)

            assert(ndivs == 0)
            ndivs = int(rer[0])
            assert(ndivs >= 0)

        if (aline.startswith("# Samples: ")): 
            rer = re.findall(r"\D(\d+)", aline)
            assert(len(rer) == 1) 

            assert(nsamples == 0)
            nsamples = int(rer[0])
            assert(nsamples > 0)

    ofile.close() 

    assert(ndivs >= 0 and nsamples > 0)

    return [ndivs, nsamples]
    

def ReportAverageBreakdown (t2first = [], i2first = [], n_restarts = []): 
    # assert(len(t2first) == len(i2first))
    srate = len(i2first)
    
    print "S. Rate: " + str(srate) + " / " + str(N_TESTS)
    if (srate > 0): 
        # assert(len(t2first) == srate)
        assert(len(i2first) == srate) 
        assert(len(n_restarts) == 0 or len(n_restarts) == srate)
        print "Average Time: " + str(float(sum(t2first)) / float(srate))
        print "Average Samples: " + str(float(sum(i2first)) / float(srate))
	if (float(sum(t2first)) >= 0):
    	    print "Average Sample Rate: " + str(float(sum(i2first)) / float(sum(t2first)))
	else: 
	    print "Average Sample Rate: Divided-by-zero..." 
        if (len(n_restarts) != 0): 
            print "Average Restarts: " + str(float(sum(n_restarts)) / float(srate))



