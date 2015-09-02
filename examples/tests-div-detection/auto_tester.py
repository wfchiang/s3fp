#!/usr/bin/env python 

import os
import sys 
import time 
import testing_utils as tu 


# ==== sub-routines ==== 
# -- simple modification of Makefile -- 
def ModifyMakefile (mfdir, opt_redefine, nvs, rseed, timeout, resource): 
    has_nvs = False 
    has_rseed = False 
    has_timeout = False 
    has_resource = False 
    has_uniform_lb = False
    has_uniform_ub = False

    mfpath = mfdir + "/Makefile" 
    mfcontent = [] 

    makefile = open(mfpath, "r") 

    for aline in makefile: 
        if (aline.startswith("N=")): 
            aline = "N=" + str(nvs) + "\n"
            has_nvs = True 
        elif (aline.startswith("RSEED=")):
            aline = "RSEED=" + str(rseed) + "\n"
            has_rseed = True 
        elif (aline.startswith("TIMEOUT=")):
            aline = "TIMEOUT=" + str(timeout) + "\n"
            has_timeout = True 
        elif (aline.startswith("RESOURCE=")):
            aline = "RESOURCE=" + str(resource) + "\n"
            has_resource = True 
        elif (aline.startswith("UNIFORM_INPUTLB=")): 
            aline = "UNIFORM_INPUTLB=" + str(tu.UNIFORM_LB) + "\n"
            has_uniform_lb = True
        elif (aline.startswith("UNIFORM_INPUTUB=")): 
            aline = "UNIFORM_INPUTUB=" + str(tu.UNIFORM_UB) + "\n" 
            has_uniform_ub = True
        else: 
            pass 
        
        mfcontent.append(aline) 
    
    makefile.close() 

    if (opt_redefine): 
        assert((not has_rseed) and (not has_timeout) and (not has_resource)) 
    else:
        assert(has_rseed and has_timeout and has_resource) 
    assert(has_nvs and has_uniform_lb and has_uniform_ub) 

    makefile = open(mfpath, "w") 

    for aline in mfcontent: 
        makefile.write(aline)

    makefile.close()

# -- simple bucket tests -- 
def TestBenchmark (mfdir, opt_redefine, tmethod, timeout, resource, nvss = []): 
    cwd = os.getcwd()

    rseeds = tu.GetRSEEDS()

    for nvs in nvss: 
        t2first = []
        i2first = []
        n_restarts = []
        i2fail = []
        srate = 0

        time_start = time.time() 

        for rseed in rseeds: 
            ModifyMakefile(mfdir, opt_redefine, nvs, rseed, timeout, resource)

            os.chdir(mfdir) 

            if tu.VERBOSE : 
                print "-- N = " + str(nvs) + ", RSEED = " + str(rseed) + " --" 

            # run s3fp 
            cmd_make = "make test-" + str(tmethod) 
            if (opt_redefine): 
                cmd_make = cmd_make + " RSEED=" + str(rseed) + " TIMEOUT=" + str(timeout) + " RESOURCE=" + str(resource) 
            cmd_make = cmd_make + " > " + tu.S3FP_OUTFILE 
            os.system(cmd_make) 
            
            thist = -1
            thisi = -1 
            thisr = -1 
            thisd = -1
            if (tmethod == "urob"): 
                thisd, thisi = tu.UROBDivOutcome(tu.S3FP_OUTFILE)
                if tu.VERBOSE :
                    print "    divs: " + str(thisd) + "    samples: " + str(thisi)
            elif (tmethod == "awbs"): 
                thist, thisi, thisr = tu.AWBSDivOutcome(tu.S3FP_OUTFILE) 
                if tu.VERBOSE : 
                    print "    time: " + str(thist) + "    samples: " + str(thisi) + "    restarts: " + str(thisr)
            elif (tmethod == "bgrt"): 
                thist, thisi = tu.BGRTDivOutcome(tu.S3FP_OUTFILE)
                if tu.VERBOSE : 
                    print "    time: " + str(thist) + "    samples: " + str(thisi) 
            else: 
                print "WARNING: No output pharsing..." 

            if (tu.EXT_TIME_MEASUREMENT): 
                thist = 0

            if (thist >= 0): 
                t2first.append(thist) 
                i2first.append(thisi) 
                n_restarts.append(thisr)
                srate = srate + 1 
            else: 
                i2fail.append(thisi)

            os.chdir(cwd)

        time_end = time.time()
        if (tu.EXT_TIME_MEASUREMENT): 
            t2first.append(float(time_end) - float(time_start))
            
        tu.ReportAverageBreakdown(t2first, i2first, n_restarts)
            
# -- test 2d point-in-triangle -- 
def Test2DPointInTriangle (tmethod, timeout, resource): 
    mfdir = "./cgal-2d-point-in-triangle" 

    print "==== test 2d point-in-triangle ====" 

    if (tmethod == "urob"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [8]) 
    elif (tmethod == "awbs"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [8])
    else: 
        print "ERROR: Invalid testing method" 


# -- test 2d triangle intersection -- 
def Test2DTriangleIntersection (tmethod, timeout, resource): 
    mfdir = "./cgal-2d-triangle-intersection" 

    print "==== test 2d triangle intersection ====" 

    if (tmethod == "urob"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [12]) 
    elif (tmethod == "awbs"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [12])
    else: 
        print "ERROR: Invalid testing method" 

# -- test convex hull -- 
def TestConvexHull (mfdir, chname, tmethod, timeout, resource): 
    print "==== test " + chname + " convex hull ====" 

    if (tmethod == "urob"): 
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [8]) 
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [200]) 
         TestBenchmark(mfdir, False, tmethod, timeout, resource, [2000]) 
    elif (tmethod == "awbs"): 
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [8])
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [200])
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [2000])
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [8, 50, 200])
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [8, 50, 200, 2000])
    elif (tmethod == "bgrt"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [50])
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [8, 50])
    else: 
        print "ERROR: Invalid testing method" 
# -- test simple convex hull -- 
def TestSimpleConvexHull (tmethod, timeout, resource): 
    TestConvexHull("./simple_convex_hull", "simple", tmethod, timeout, resource)
def TestSimpleConvexHullWeaksig (tmethod, timeout, resource): 
    TestConvexHull("./simple_convex_hull_weaksig", "simple weaksig", tmethod, timeout, resource)
# -- test incremental convex hull -- 
def TestIncrementalConvexHull (tmethod, timeout, resource): 
    TestConvexHull("./inc_convex_hull", "incremental", tmethod, timeout, resource)
def TestIncrementalConvexHullWeaksig (tmethod, timeout, resource): 
    TestConvexHull("./inc_convex_hull_weaksig", "incremental weaksig", tmethod, timeout, resource)
# -- test bykat convex hull -- 
def TestBykatConvexHull (tmethod, timeout, resource): 
    TestConvexHull("./cgal-convex-hull-byk", "bykat", tmethod, timeout, resource)
def TestBykatConvexHullWeaksig (tmethod, timeout, resource): 
    TestConvexHull("./cgal-convex-hull-byk-weaksig", "bykat weaksig", tmethod, timeout, resource)
# -- test graham convex hull -- 
def TestGrahamConvexHull (tmethod, timeout, resource): 
    TestConvexHull("./cgal-convex-hull-gra", "graham", tmethod, timeout, resource)
def TestGrahamConvexHullWeaksig (tmethod, timeout, resource): 
    TestConvexHull("./cgal-convex-hull-gra-weaksig", "graham weaksig", tmethod, timeout, resource)

# -- test pint-to-plane -- 
def TestPP (mfdir, oname, tmethod, timeout, resource): 
    print "==== test pp " + oname + " ====" 

    if (tmethod == "urob" or tmethod == "bgrt" or tmethod == "awbs"): 
        TestBenchmark(mfdir, True, tmethod, timeout, resource, [12]) 
    else: 
        print "ERROR: Invalid testing method" 
# -- test point-to-plane 3x3 -- 
def TestPP3x3 (tmethod, timeout, resource): 
    TestPP("./pp_3x3", "3x3", tmethod, timeout, resource) 
# -- test point-to-plane 4x4 -- 
def TestPP4x4 (tmethod, timeout, resource): 
    TestPP("./pp_4x4", "4x4", tmethod, timeout, resource) 

# -- test point-to-circle -- 
def TestPC (mfdir, oname, tmethod, timeout, resource): 
    print "==== test point-to-circle " + oname + " ====" 

    if (tmethod == "urob" or tmethod == "awbs" or tmethod == "bgrt"):  
        TestBenchmark(mfdir, True, tmethod, timeout, resource, [8]) 
    else: 
        print "ERROR: Invalid testing method" 
# -- test point-to-circle 3x3 -- 
def TestPC3x3 (tmethod, timeout, resource): 
    TestPC("./pc_3x3", "3x3", tmethod, timeout, resource) 
# -- test point-to-circle 4x4 -- 
def TestPC4x4 (tmethod, timeout, resource): 
    TestPC("./pc_4x4", "4x4", tmethod, timeout, resource) 

# -- test point-to-sphere -- 
def TestPS (mfdir, oname, tmethod, timeout, resource): 
    print "==== test point-to-sphere " + oname + " ====" 

    if (tmethod == "urob" or tmethod == "awbs" or tmethod == "bgrt"):  
        TestBenchmark(mfdir, True, tmethod, timeout, resource, [15]) 
    else: 
        print "ERROR: Invalid testing method" 
# -- test point-to-sphere 4x4 -- 
def TestPS4x4 (tmethod, timeout, resource): 
    TestPS("./ps_4x4", "4x4", tmethod, timeout, resource) 
# -- test point-to-sphere 5x5 -- 
def TestPS5x5 (tmethod, timeout, resource): 
    TestPS("./ps_5x5", "5x5", tmethod, timeout, resource) 

# -- test shortest-path -- 
def TestShortestPath (tmethod, timeout, resource): 
    mfdir = "./shortest-path"

    print "==== test shortest-path ====" 

    if (tmethod == "urob"): 
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [12])
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [90, 600])
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [2450])
    elif (tmethod == "awbs"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [12, 90])
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [600, 2450])
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [12, 90, 600, 2450])
    else: 
        print "ERROR: Invalid testing method" 

# ---- made-up histogram v0 ---- 
def TestMadeUpHistogramV0 (mfdir, oname, tmethod, timeout, resource): 
    print "==== test made-up histogram v0 (" + oname + ") ====" 

    if (tmethod == "urob" or tmethod == "awbs"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [1024])
# ---- made-up histogram v0 abs ---- 
def TestMadeUpHistogramV0Abs (tmethod, timeout, resource): 
    TestMadeUpHistogramV0("./made-up-histogram-v0-abs", "abs", tmethod, timeout, resource) 
# ---- made-up histogram v0 rel ---- 
def TestMadeUpHistogramV0Rel (tmethod, timeout, resource): 
    TestMadeUpHistogramV0("./made-up-histogram-v0-rel", "rel", tmethod, timeout, resource) 
# ---- made-up histogram v0 absrel ---- 
def TestMadeUpHistogramV0Absrel (tmethod, timeout, resource): 
    TestMadeUpHistogramV0("./made-up-histogram-v0-absrel", "absrel", tmethod, timeout, resource)     

# ---- made-up reduction-equal v0 ---- 
def TestMadeUpReductionEqualV0 (mfdir, oname, tmethod, timeout, resource): 
    print "==== test made-up reduction-equal v0 (" + oname + ") ====" 

    if (tmethod == "urob" or tmethod == "awbs"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [256])
# ---- made-up reduction-equal v0 abs ---- 
def TestMadeUpReductionEqualV0Abs (tmethod, timeout, resource): 
    TestMadeUpReductionEqualV0("./made-up-reduction-equal-v0-abs", "abs", tmethod, timeout, resource) 
# ---- made-up reduction-equal v0 rel ---- 
def TestMadeUpReductionEqualV0Rel (tmethod, timeout, resource): 
    TestMadeUpReductionEqualV0("./made-up-reduction-equal-v0-rel", "rel", tmethod, timeout, resource) 
# ---- made-up reduction-equal v0 absrel ---- 
def TestMadeUpReductionEqualV0Absrel (tmethod, timeout, resource): 
    TestMadeUpReductionEqualV0("./made-up-reduction-equal-v0-absrel", "absrel", tmethod, timeout, resource)     

# -- test variance estimation -- 
def TestVarEstimation (tmethod, timeout, resource): 
    mfdir = "./var_estimation"

    print "==== test var. estimation ====" 

    if (tmethod == "urob"): 
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [100]) 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [1000]) 
    elif (tmethod == "bgrt" or tmethod == "opt4j"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [100, 1000, 10000]) 
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [10000, 1000]) 
    else: 
        print "ERROR: Invalid testing method" 

# -- test scan -- 
def TestScan (mfdir, sname, optmethod, tmethod, timeout, resource): 
    print "==== test scan " + sname + " with opt: " + optmethod + " ===="

    if (tmethod == "urob"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [128]) 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [1024]) 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [8192]) 
    elif (tmethod == "bgrt"): 
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [128])
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [1024])
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [8192])
    elif (tmethod == "opt4j"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [128, 8192])
    else: 
        print "ERROR: Invalid testing method" 
# -- test naive scan with opt: last -- 
def TestNaiveScanOptLast (tmethod, timeout, resource): 
    TestScan("./cpu-scan-naive-opt-last", "naive", "last", tmethod, timeout, resource)
# -- test naive scan with opt: sum -- 
def TestNaiveScanOptSum (tmethod, timeout, resource): 
    TestScan("./cpu-scan-naive-opt-sum", "naive", "sum", tmethod, timeout, resource)
# -- test harris scan with opt: last -- 
def TestHarrisScanOptLast (tmethod, timeout, resource): 
    TestScan("./cpu-scan-harris-opt-last", "harris", "last", tmethod, timeout, resource)
# -- test harris scan with opt: sum -- 
def TestHarrisScanOptSum (tmethod, timeout, resource): 
    TestScan("./cpu-scan-harris-opt-sum", "harris", "sum", tmethod, timeout, resource)

# -- test sorting -- 
def TestSorting (mfdir, sname, tmethod, timeout, resource): 
    print "==== test sorting " + sname + " ====" 

    if (tmethod == "urob"): 
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [256]) 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [4096]) 
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [10000])
    elif (tmethod == "bgrt"): 
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [256]) 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [4096]) 
#        TestBenchmark(mfdir, False, tmethod, timeout, resource, [10000])
    else: 
        print "ERROR: Invalid testing method"
# -- test perfect sorting: last -- 
def TestPerfSortingLast (tmethod, timeout, resource): 
    TestSorting("./sorting-perf-last", "perfect+last", tmethod, timeout, resource)
# -- test perfect sorting: sum -- 
def TestPerfSortingSum (tmethod, timeout, resource): 
    TestSorting("./sorting-perf-sum", "perfect+sum", tmethod, timeout, resource)
# -- test sorting with permutations allowed: last -- 
def TestPermSortingLast (tmethod, timeout, resource): 
    TestSorting("./sorting-perm-last", "permutation+last", tmethod, timeout, resource)
# -- test sorting with permutations allowed: sum -- 
def TestPermSortingSum (tmethod, timeout, resource): 
    TestSorting("./sorting-perm-sum", "permutation+sum", tmethod, timeout, resource)


# -- test 3d line intersect with 2 triangles -- 
def Test3DLT2AT (tmethod, timeout, resource): 
    mfdir = "./cgal-3d-line-through-2-adjacent-triangles" 

    print "==== test 3d line intersect with 2 triangles ====" 

    if (tmethod == "urob" or tmethod == "bgrt" or tmethod == "awbs"): 
        TestBenchmark(mfdir, False, tmethod, timeout, resource, [18]) 
    else: 
        print "ERROR: Invalid testing method" 



# ==== main ==== 
tu.HandleSpecialUsage(sys.argv)

if (len(sys.argv) != 7): 
    tu.WrongUsage(7)

# -- check arguments -- 
tu.SetGlobalVariables(sys.argv) 

# -- load RSEED setting -- 
tu.LoadRSEEDSetting()
assert(tu.LAST_RSEED >= 0 and tu.RSEED_GAP > 0)

# -- set verbose -- 
tu.VERBOSE = True

# -- tests -- 
# Test2DPointInTriangle (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
# Test2DTriangleIntersection (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
# TestSimpleConvexHull (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 
# TestBykatConvexHull (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 
# TestIncrementalConvexHull (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 
# TestGrahamConvexHull (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 
TestPP3x3 (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
TestPP4x4 (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
TestPC3x3 (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
TestPC4x4 (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 
TestPS4x4 (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
TestPS5x5 (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 
# TestShortestPath (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 

# TestSimpleConvexHullWeaksig (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 
# TestIncrementalConvexHullWeaksig (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 
# TestBykatConvexHullWeaksig (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 
# TestGrahamConvexHullWeaksig (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 

# TestMadeUpHistogramV0Abs (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
# TestMadeUpHistogramV0Rel (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
# TestMadeUpHistogramV0Absrel (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
# TestMadeUpReductionEqualV0Abs (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
# TestMadeUpReductionEqualV0Rel (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
# TestMadeUpReductionEqualV0Absrel (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)

# TestVarEstimation (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 

# TestNaiveScanOptLast (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)  
# TestHarrisScanOptLast (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 
# TestNaiveScanOptSum (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 
# TestHarrisScanOptSum (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)

# TestPerfSortingLast (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
# TestPerfSortingSum (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
# TestPermSortingLast (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)
# TestPermSortingSum (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE)

# Test3DLT2AT (tu.TMETHOD, tu.TIMEOUT, tu.RESOURCE) 


