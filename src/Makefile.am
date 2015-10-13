
# compilation flags 
CFLAGS = -std=c++0x -Wno-unused-result 
AM_LDFLAGS = 

# binary target 
bin_PROGRAMS = s3fp s3fp_it128

# for compiling s3fp 
s3fp_SOURCES = S3FP_ParseArgs.cpp S3FP_ParseArgs.h S3FP_ErrorFunction.cpp S3FP_ErrorFunction.h S3FP_OutputParser.cpp S3FP_OutputParser.h S3FP_ErrorOptimization.cpp S3FP_ErrorOptimization.h S3FP_SignatureGen.cpp S3FP_SignatureGen.h S3FP_DifferentialContract.cpp S3FP_DifferentialContract.h S3FP_Core.cpp S3FP_Core.h 
s3fp_LDFLAGS = 
s3fp_LDADD = -lpthread -lquadmath 

# for compiling s3fp_it128 
# binary target for __float128 as the input type 
s3fp_it128_SOURCES = S3FP_ParseArgs.cpp S3FP_ParseArgs.h S3FP_ErrorFunction.cpp S3FP_ErrorFunction.h S3FP_OutputParser.cpp S3FP_OutputParser.h S3FP_ErrorOptimization.cpp S3FP_ErrorOptimization.h S3FP_SignatureGen.cpp S3FP_SignatureGen.h S3FP_DifferentialContract.cpp S3FP_DifferentialContract.h S3FP_Core.cpp S3FP_Core.h 
s3fp_it128_CFLAGS = -DINPUTV_TYPE=__float128 
s3fp_it128_LDFLAGS = 
s3fp_it128_LDADD = -lpthread -lquadmath 

