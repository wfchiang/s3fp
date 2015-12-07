
# binary targets 
bin_PROGRAMS = kahan_sum_32 kahan_sum_64 kahan_sum_128 

# ==== examples ====

kahan_sum_32_SOURCES = kahan_sum.cpp 
kahan_sum_32_CPPFLAGS = -DFT=float 
kahan_sum_32_LDFLAGS = 
kahan_sum_32_LDADD = -lquadmath 

kahan_sum_64_SOURCES = kahan_sum.cpp 
kahan_sum_64_CPPFLAGS = -DFT=double 
kahan_sum_64_LDFLAGS = 
kahan_sum_64_LDADD = -lquadmath 

kahan_sum_128_SOURCES = kahan_sum.cpp 
kahan_sum_128_CPPFLAGS = -DFT=__float128 
kahan_sum_128_LDFLAGS = 
kahan_sum_128_LDADD = -lquadmath 