
#include <stdio.h>
#include "div_geometric_primitives.h"


/*
  matrix determinant
*/

template<class DFT> 
DFT *SMatrix (int dim, int x, int y, DFT *matrix) {
  assert(dim > 1); 
  assert(dim > x);
  assert(dim > y); 

  DFT * retm = (DFT *) malloc(sizeof(DFT)*(dim-1)*(dim-1));
  int ir = 0; 

  for (int ix = 0 ; ix < dim ; ix++) {
    for (int iy = 0 ; iy < dim ; iy++) {
      if (ix == x) continue; 
      else if (iy == y) continue; 
      else {
	retm[ir] = matrix[ix*dim + iy]; 
	ir++; 
      }
    }
  }
  
  return retm; 
}


template<class DFT> 
DFT MDeterminant (int dim, DFT *matrix) {
  assert(dim > 0); 
  if (dim == 1) return matrix[0]; 
  else {
    DFT ** sms = (DFT**) malloc(sizeof(DFT*) * dim); 

    for (int iy = 0 ; iy < dim ; iy++) 
      sms[iy] = SMatrix<DFT>(dim, 0, iy, matrix);  
    
    DFT retd = 0.0; 
    for (int iy = 0 ; iy < dim ; iy++) {
      if (iy % 2 == 0)
	retd += matrix[0*dim+iy] * MDeterminant((dim-1), sms[iy]);
      else // (iy % 2 == 0)
	retd -= matrix[0*dim+iy] * MDeterminant((dim-1), sms[iy]);
    } 

    for (int iy = 0 ; iy < dim ; iy++) 
      free(sms[iy]);
    free(sms);

    return retd; 
  }

  assert(false); 
}


float matrix_determinant_32 (int dim, void *matrix) {
  return MDeterminant<float>(dim, (float*)matrix); 
}


double matrix_determinant_64 (int dim, void *matrix) {
  return MDeterminant<double>(dim, (double*)matrix); 
}


__float128 matrix_determinant_128 (int dim, void *matrix) {
  return MDeterminant<__float128>(dim, (__float128*)matrix);
}


/* 
   insphere test 
*/

template<class FT> 
FT Insphere_4x4 (FT ax, FT ay, FT az, 
		 FT bx, FT by, FT bz, 
		 FT cx, FT cy, FT cz, 
		 FT dx, FT dy, FT dz, 
		 FT px, FT py, FT pz) {

  FT *detm = (FT*) malloc(sizeof(FT) * 16); 

  detm[0] = ax - px; 
  detm[1] = ay - py;
  detm[2] = az - pz;
  detm[3] = ((detm[0]*detm[0]) + (detm[1]*detm[1])) + (detm[2]*detm[2]);

  detm[4] = bx - px; 
  detm[5] = by - py;
  detm[6] = bz - pz;
  detm[7] = ((detm[4]*detm[4]) + (detm[5]*detm[5])) + (detm[6]*detm[6]);

  detm[8] = cx - px; 
  detm[9] = cy - py;
  detm[10] = cz - pz;
  detm[11] = ((detm[8]*detm[8]) + (detm[9]*detm[9])) + (detm[10]*detm[10]);

  detm[12] = dx - px; 
  detm[13] = dy - py;
  detm[14] = dz - pz;
  detm[15] = ((detm[12]*detm[12]) + (detm[13]*detm[13])) + (detm[14]*detm[14]);

  return MDeterminant<FT>(4, detm);
}

template<class FT> 
FT Insphere_5x5 (FT ax, FT ay, FT az, 
		 FT bx, FT by, FT bz, 
		 FT cx, FT cy, FT cz, 
		 FT dx, FT dy, FT dz, 
		 FT px, FT py, FT pz) {

  FT *detm = (FT*) malloc(sizeof(FT) * 25); 

  detm[0] = ax; 
  detm[1] = ay;
  detm[2] = az;
  detm[3] = ((ax*ax) + (ay*ay)) + (az*az);
  detm[4] = 1.0; 

  detm[5] = bx; 
  detm[6] = by;
  detm[7] = bz;
  detm[8] = ((bx*bx) + (by*by)) + (bz*bz);
  detm[9] = 1.0; 

  detm[10] = cx; 
  detm[11] = cy;
  detm[12] = cz;
  detm[13] = ((cx*cx) + (cy*cy)) + (cz*cz);
  detm[14] = 1.0; 

  detm[15] = dx; 
  detm[16] = dy;
  detm[17] = dz;
  detm[18] = ((dx*dx) + (dy*dy)) + (dz*dz);
  detm[19] = 1.0; 

  detm[20] = px; 
  detm[21] = py;
  detm[22] = pz;
  detm[23] = ((px*px) + (py*py)) + (pz*pz);
  detm[24] = 1.0; 

  return MDeterminant<FT>(5, detm);
}


float insphere_32 (int det_dimension, 
		   float ax, float ay, float az, 
		   float bx, float by, float bz, 
		   float cx, float cy, float cz, 
		   float dx, float dy, float dz, 
		   float px, float py, float pz) {
  if (det_dimension == 4)
    return Insphere_4x4<float>(ax, ay, az,
			       bx, by, bz, 
			       cx, cy, cz, 
			       dx, dy, dz, 
			       px, py, pz);
  else if (det_dimension == 5) 
    return Insphere_5x5<float>(ax, ay, az,
			       bx, by, bz, 
			       cx, cy, cz, 
			       dx, dy, dz, 
			       px, py, pz);
  else {
    assert(false && "ERROR: Invalid dimension");
    return 0; 
  }
}


double insphere_64 (int det_dimension, 
		    double ax, double ay, double az, 
		    double bx, double by, double bz, 
		    double cx, double cy, double cz, 
		    double dx, double dy, double dz, 
		    double px, double py, double pz) {
  if (det_dimension == 4)
    return Insphere_4x4<double>(ax, ay, az,
				bx, by, bz, 
				cx, cy, cz, 
				dx, dy, dz, 
				px, py, pz);
  else if (det_dimension == 5)
    return Insphere_5x5<double>(ax, ay, az,
				bx, by, bz, 
				cx, cy, cz, 
				dx, dy, dz, 
				px, py, pz);
  else {
    assert(false && "ERROR: Invalid dimension");
    return 0;
  } 
}


__float128 insphere_128 (int det_dimension, 
			 __float128 ax, __float128 ay, __float128 az, 
			 __float128 bx, __float128 by, __float128 bz, 
			 __float128 cx, __float128 cy, __float128 cz, 
			 __float128 dx, __float128 dy, __float128 dz, 
			 __float128 px, __float128 py, __float128 pz) {
  if (det_dimension == 4)
    return Insphere_4x4<__float128>(ax, ay, az,
				    bx, by, bz, 
				    cx, cy, cz, 
				    dx, dy, dz, 
				    px, py, pz);
  else if (det_dimension == 5) 
    return Insphere_5x5<__float128>(ax, ay, az,
				    bx, by, bz, 
				    cx, cy, cz, 
				    dx, dy, dz, 
				    px, py, pz);
  else {
    assert(false && "ERROR: Invalid dimension");
    return 0;
  } 
}

