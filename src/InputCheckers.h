#include "definitions.h"

#define MIN_DISTANCE_2 0.001
#define MIN_GRADIENT 0.0001

using namespace std; 


inline 
bool 
CloseToZero (INPUTV_TYPE gra, INPUTV_TYPE zrange) 
{
  assert(zrange > 0); 
  if (((-1*zrange) < gra) && 
      (gra < zrange)) return true; 
  return false; 
}


bool 
IC_farAwayPoints2D (INPUTV_TYPE ax, INPUTV_TYPE ay, INPUTV_TYPE bx, INPUTV_TYPE by) 
{
  if ((((bx - ax) * (bx - ax)) + ((by - ay) * (by - ay))) 
      < MIN_DISTANCE_2) return false;
  
  return true; 
}


bool 
IC_farAwayPoints3D (INPUTV_TYPE ax, INPUTV_TYPE ay, INPUTV_TYPE az, 
		    INPUTV_TYPE bx, INPUTV_TYPE by, INPUTV_TYPE bz) 
{
  if (((((ax - bx) * (ax - bx)) 
	+ ((ay - by) * (ay - by))) 
       + ((az - bz) * (az - bz))) 
      < MIN_DISTANCE_2) {
    return false;
  }
  
  return true; 
}


bool 
IC_formALine2D (INPUTV_TYPE ax, INPUTV_TYPE ay, 
		INPUTV_TYPE bx, INPUTV_TYPE by, 
		INPUTV_TYPE cx, INPUTV_TYPE cy) 
{
  INPUTV_TYPE diff_ba_x = bx - ax;
  INPUTV_TYPE diff_ba_y = by - ay; 
  INPUTV_TYPE diff_ca_x = cx - ax;
  INPUTV_TYPE diff_ca_y = cy - ay; 
  INPUTV_TYPE gradient_ba; 
  INPUTV_TYPE gradient_ca; 
  INPUTV_TYPE diff_gradient; 

  if (CloseToZero(diff_ba_x, MIN_GRADIENT)) {
    if (CloseToZero(diff_ca_x, MIN_GRADIENT)) return true; 
  }
  else {
    if (CloseToZero(diff_ca_x, MIN_GRADIENT) == false) {
      gradient_ba = diff_ba_y / diff_ba_x; 
      gradient_ca = diff_ca_y / diff_ca_x;   
      if (gradient_ba * gradient_ca > 0) {
	diff_gradient = gradient_ca - gradient_ba; 
	if (CloseToZero(diff_gradient, MIN_GRADIENT)) return true;
      }
    }
  }
  
  return false; 
}


bool 
IC_formALine3D (INPUTV_TYPE ax, INPUTV_TYPE ay, INPUTV_TYPE az, 
		INPUTV_TYPE bx, INPUTV_TYPE by, INPUTV_TYPE bz, 
		INPUTV_TYPE cx, INPUTV_TYPE cy, INPUTV_TYPE cz) 
{
  if (IC_formALine2D(ax, ay, bx, by, cx, cy) && 
      IC_formALine2D(ax, az, bx, bz, cx, cz) && 
      IC_formALine2D(ay, az, by, bz, by, bz)) return true; 
  return false; 
}


bool 
IC_formAPlane (vector<INPUTV_TYPE> ivalues) 
{

  INPUTV_TYPE min_distance_2 = 0.001;
  INPUTV_TYPE ax = ivalues[0];
  INPUTV_TYPE ay = ivalues[1];
  INPUTV_TYPE az = ivalues[2];
  INPUTV_TYPE bx = ivalues[3];
  INPUTV_TYPE by = ivalues[4];
  INPUTV_TYPE bz = ivalues[5];
  INPUTV_TYPE cx = ivalues[6];
  INPUTV_TYPE cy = ivalues[7];
  INPUTV_TYPE cz = ivalues[8];
  
  // check between a and b 
  if (IC_farAwayPoints3D(ax, ay, az, bx, by, bz) == false) {
    cout << "Close Points: " << endl;
    cout << "[" << (long double) ax << ", " << (long double) ay << ", " << (long double) az << "]" << endl;
    cout << "[" << (long double) bx << ", " << (long double) by << ", " << (long double) bz << "]" << endl;
    return false;
  }
  // check between b and c 
  if (IC_farAwayPoints3D(bx, by, bz, cx, cy, cz) == false) {
    cout << "Close Points: " << endl;
    cout << "[" << (long double) bx << ", " << (long double) by << ", " << (long double) bz << "]" << endl;    
    cout << "[" << (long double) cx << ", " << (long double) cy << ", " << (long double) cz << "]" << endl;
    return false; 
  }
  // check between a and c 
  if (IC_farAwayPoints3D(ax, ay, az, cx, cy, cz) == false) {
    cout << "Close Points: " << endl;
    cout << "[" << (long double) ax << ", " << (long double) ay << ", " << (long double) az << "]" << endl;
    cout << "[" << (long double) cx << ", " << (long double) cy << ", " << (long double) cz << "]" << endl;
    return false; 
  }

  if (IC_formALine3D(ax, ay, az, bx, by, bz, cx, cy, cz)) return false;

  return true;
}
