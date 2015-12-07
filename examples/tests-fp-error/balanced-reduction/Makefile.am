
# binary targets 
bin_PROGRAMS = balanced_reduction_32 balanced_reduction_64 balanced_reduction_128 

# ==== examples ====

balanced_reduction_32_SOURCES = balanced_reduction.cpp 
balanced_reduction_32_CFLAGS = -DFT=float 
balanced_reduction_32_LDFLAGS = 
balanced_reduction_32_LDADD = -lquadmath 

balanced_reduction_64_SOURCES = balanced_reduction.cpp 
balanced_reduction_64_CFLAGS = -DFT=double 
balanced_reduction_64_LDFLAGS = 
balanced_reduction_64_LDADD = -lquadmath 

balanced_reduction_128_SOURCES = balanced_reduction.cpp 
balanced_reduction_128_CFLAGS = -DFT=__float128 
balanced_reduction_128_LDFLAGS = 
balanced_reduction_128_LDADD = -lquadmath 