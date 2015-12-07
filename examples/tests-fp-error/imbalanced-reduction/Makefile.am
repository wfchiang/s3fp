
# binary targets 
bin_PROGRAMS = imbalanced_reduction_32 imbalanced_reduction_64 imbalanced_reduction_128 

# ==== examples ====

imbalanced_reduction_32_SOURCES = imbalanced_reduction.cpp 
imbalanced_reduction_32_CPPFLAGS = -DFT=float 
imbalanced_reduction_32_LDFLAGS = 
imbalanced_reduction_32_LDADD = -lquadmath 

imbalanced_reduction_64_SOURCES = imbalanced_reduction.cpp 
imbalanced_reduction_64_CPPFLAGS = -DFT=double 
imbalanced_reduction_64_LDFLAGS = 
imbalanced_reduction_64_LDADD = -lquadmath 

imbalanced_reduction_128_SOURCES = imbalanced_reduction.cpp 
imbalanced_reduction_128_CPPFLAGS = -DFT=__float128 
imbalanced_reduction_128_LDFLAGS = 
imbalanced_reduction_128_LDADD = -lquadmath 