
# binary targets 
bin_PROGRAMS = cpu_scan_32 cpu_scan_64 cpu_scan_128 

# ==== examples ====

cpu_scan_32_SOURCES = cpu_scan.cpp 
cpu_scan_32_CPPFLAGS = -DWFT=float -I../../include 
cpu_scan_32_LDFLAGS = 
cpu_scan_32_LDADD = -lquadmath 

cpu_scan_64_SOURCES = cpu_scan.cpp 
cpu_scan_64_CPPFLAGS = -DWFT=double -I../../include 
cpu_scan_64_LDFLAGS = 
cpu_scan_64_LDADD = -lquadmath 

cpu_scan_128_SOURCES = cpu_scan.cpp 
cpu_scan_128_CPPFLAGS = -DWFT=__float128 -I../../include 
cpu_scan_128_LDFLAGS = 
cpu_scan_128_LDADD = -lquadmath 