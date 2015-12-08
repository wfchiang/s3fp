
# binary targets 
bin_PROGRAMS = cpu_scan_32 cpu_scan_64 cpu_scan_128 

# ==== examples ====

cpu_scan_32_SOURCES = cpu_scan.cpp 
cpu_scan_32_CPPFLAGS = -DWFT=float -I../../include -I../../lib/div_geometric_primitives 
cpu_scan_32_LDFLAGS = -L../../lib/div_geometric_primitives 
cpu_scan_32_LDADD = -ldiv_geometric_primitives -lquadmath 

cpu_scan_64_SOURCES = cpu_scan.cpp 
cpu_scan_64_CPPFLAGS = -DWFT=double -I../../include -I../../lib/div_geometric_primitives 
cpu_scan_64_LDFLAGS = -L../../lib/div_geometric_primitives 
cpu_scan_64_LDADD = -ldiv_geometric_primitives -lquadmath 

cpu_scan_128_SOURCES = cpu_scan.cpp 
cpu_scan_128_CPPFLAGS = -DWFT=__float128 -I../../include -I../../lib/div_geometric_primitives 
cpu_scan_128_LDFLAGS = -L../../lib/div_geometric_primitives 
cpu_scan_128_LDADD = -ldiv_geometric_primitives -lquadmath 