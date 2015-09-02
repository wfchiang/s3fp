#include <assert.h>
extern "C" {
#include "quadmath.h"
}

// for matrix determinant 

float matrix_determinant_32 (int dim, void *matrix); 

double matrix_determinant_64 (int dim, void *matrix); 

__float128 matrix_determinant_128 (int dim, void *matrix); 

// for insphere test

float insphere_32 (int det_dimension, 
		   float ax, float ay, float az, 
		   float bx, float by, float bz, 
		   float cx, float cy, float cz, 
		   float dx, float dy, float dz, 
		   float px, float py, float pz); 

double insphere_64 (int det_dimension, 
		    double ax, double ay, double az, 
		    double bx, double by, double bz, 
		    double cx, double cy, double cz, 
		    double dx, double dy, double dz, 
		    double px, double py, double pz); 

__float128 insphere_128 (int det_dimension, 
			 __float128 ax, __float128 ay, __float128 az, 
			 __float128 bx, __float128 by, __float128 bz, 
			 __float128 cx, __float128 cy, __float128 cz, 
			 __float128 dx, __float128 dy, __float128 dz, 
			 __float128 px, __float128 py, __float128 pz); 

