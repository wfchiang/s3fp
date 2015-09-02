#include <stdio.h>
#include <assert.h>


FILE * 
s3fpGetInFile (int argc, char **argv); 


FILE * 
s3fpGetOutFile (int argc, char **argv); 


unsigned long 
s3fpFileSize (FILE *f); 


void 
s3fpGetInputs32 (FILE *infile, 
		 unsigned long n_inputs, 
		 float *iarr); 


void 
s3fpWriteOutputs32 (FILE *outfile, 
		    unsigned int n_outputs, 
		    float *oarr); 


void 
s3fpWriteOutputs64 (FILE *outfile, 
		    unsigned int n_outputs, 
		    double *oarr); 
