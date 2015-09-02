#include <stdio.h>
#include <iostream>
#include <vector>
#include <iterator> 
#include <assert.h>
#include <math.h>

#include "cgal_precision_selection.h"
#include "s3fp_utils.h"

using namespace std; 

#ifndef IFT
#define IFT float 
#endif 

#ifndef OFT
#define OFT __float128
#endif 


void PrintPoint3 (Point_3 p3) {
  cout << "(" << p3.x() << ", " << p3.y() << ", " << p3.z() << ")"; 
}


bool PinTriangle3D (Point_3 triA, Point_3 triB, Point_3 triC, 
		    Point_3 tp) {
  Triangle_2 proTriangleABC (Point_2(triA.x(), triA.y()), 
			     Point_2(triB.x(), triB.y()), 
			     Point_2(triC.x(), triC.y()));

  Point_2 tp_2d (tp.x(), tp.y()); 

  if (proTriangleABC.has_on_boundary(tp_2d) || 
      proTriangleABC.has_on_bounded_side(tp_2d)) return true; 
  else return false; 
}


/* 
   vmethod == 0 : strong validation method ==> use HIGH_IMPLIES_LOW
   vmethod == 1 : week validation method ==> use HIGH_IMPLIES_LOW
   vmethod == 2 : FIVE-STAGE for abs 
 */
int main (int argc, char *argv[]) {
  assert(argc == 4); 
  
  unsigned int vmethod = atoi(argv[1]); 
  assert(vmethod == 0); 

  FILE *infile = s3fpGetInFile(argc, argv); 
  FILE *outfile = s3fpGetOutFile(argc, argv); 

  unsigned long fsize = s3fpFileSize(infile); 
  assert(fsize % (sizeof(IFT) * 18) == 0); 

  IFT triA_x, triA_y, triA_z; 
  IFT triB_x, triB_y, triB_z; 
  IFT triC_x, triC_y, triC_z; 
  IFT triD_x, triD_y, triD_z; 

  IFT lineX_x, lineX_y, lineX_z; 
  IFT lineY_x, lineY_y, lineY_z; 

  fread(&triA_x, sizeof(IFT), 1, infile); 
  fread(&triA_y, sizeof(IFT), 1, infile); 
  fread(&triA_z, sizeof(IFT), 1, infile); 

  fread(&triB_x, sizeof(IFT), 1, infile); 
  fread(&triB_y, sizeof(IFT), 1, infile); 
  fread(&triB_z, sizeof(IFT), 1, infile); 

  fread(&triC_x, sizeof(IFT), 1, infile); 
  fread(&triC_y, sizeof(IFT), 1, infile); 
  fread(&triC_z, sizeof(IFT), 1, infile); 

  fread(&triD_x, sizeof(IFT), 1, infile); 
  fread(&triD_y, sizeof(IFT), 1, infile); 
  fread(&triD_z, sizeof(IFT), 1, infile); 

  fread(&lineX_x, sizeof(IFT), 1, infile);
  fread(&lineX_y, sizeof(IFT), 1, infile);
  fread(&lineX_z, sizeof(IFT), 1, infile);

  fread(&lineY_x, sizeof(IFT), 1, infile);
  fread(&lineY_y, sizeof(IFT), 1, infile);
  fread(&lineY_z, sizeof(IFT), 1, infile);

  // --------
  /*
  Point_3 triA (1.01, 0.0, 0.0); 
  Point_3 triB (0.0, 1.02, 0.0); 
  Point_3 triC (1.04, 1.07, 0.0); 
  Point_3 triD (0.0, 0.0, 1.11); 

  Point_3 liX (0.5, 0.5, 0);
  Point_3 liY (0.5, 0.5, 1); 
  */
  
  Point_3 triA (triA_x, triA_y, triA_z); 
  Point_3 triB (triB_x, triB_y, triB_z);  
  Point_3 triC (triC_x, triC_y, triC_z); 
  Point_3 triD (triD_x, triD_y, triD_z);  

  Point_3 liX (lineX_x, lineX_y, lineX_z); 
  Point_3 liY (lineY_x, lineY_y, lineY_z); 
  
  // --------

  Plane_3 planeABC (triA, triB, triC); 
  Plane_3 planeBAD (triA, triB, triD); 

  Line_3 lineL (liX, liY); 
  
  auto inter_ABCL = CGAL::intersection(planeABC, lineL); 
  auto inter_BADL = CGAL::intersection(planeBAD, lineL); 
  
  bool isInTriangleABC = false; 
  bool isInTriangleBAD = false; 

  WF_ORIENT_LOG_ERROR = true; 
  // WF_ORIENT_VERBOSE = true; 
  WF_ORIENT_ERRFILE = outfile; 

  OFT n_err = 12; 
  fwrite(&n_err, sizeof(OFT), 1, outfile); 
  OFT dummy_errs[6]; 
  dummy_errs[0] = dummy_errs[1] = dummy_errs[2] = dummy_errs[3] = dummy_errs[4] = dummy_errs[5] = 0.0; 
  unsigned int n_fw_abc = 0; 

  if (inter_ABCL) {
    const Point_3 *abcl_interp = boost::get<Point_3>(&*inter_ABCL); 
    if (abcl_interp != NULL) {
      cout << "plane ABC and L intersect on (" << abcl_interp->x() << ", " << abcl_interp->y() << ", " << abcl_interp->z() << ")" << endl;

      isInTriangleABC = PinTriangle3D(triA, triB, triC, (*abcl_interp)); 

      if (isInTriangleABC) 
	cout << "L pass through triangle ABC..." << endl;
      else 
	cout << "L outside triangle ABC..." << endl;

      n_fw_abc = WF_ORIENT_N; 

      for (unsigned int i = 0 ; i < (6 - n_fw_abc) ; i++) {
	OFT dummy = 0.0; 
	fwrite(&dummy, sizeof(OFT), 1, outfile); 
      }
    }
    else {
      s3fpWriteOutputs<OFT>(outfile, 6, dummy_errs); 
      cout << "ABC and L are not intersect on a point..." << endl;
    }
  }
  else {
    s3fpWriteOutputs<OFT>(outfile, 6, dummy_errs); 
    cout << "ABC and L are not intersect at all..." << endl;
  }

  if (inter_BADL) {
    const Point_3 *abdl_interp = boost::get<Point_3>(&*inter_BADL); 
    if (abdl_interp != NULL) {
      cout << "plane BAD and L intersect on (" << abdl_interp->x() << ", " << abdl_interp->y() << ", " << abdl_interp->z() << ")" << endl;

      isInTriangleBAD = PinTriangle3D(triB, triA, triD, (*abdl_interp)); 

      if (isInTriangleBAD) 
	cout << "L pass through triangle BAD..." << endl;
      else 
	cout << "L outside triangle BAD..." << endl;

      for (unsigned int i = 0 ; i < (6 - (WF_ORIENT_N - n_fw_abc)) ; i++) {
	OFT dummy = 0.0; 
	fwrite(&dummy, sizeof(OFT), 1, outfile); 
      }
    }
    else {
      s3fpWriteOutputs<OFT>(outfile, 6, dummy_errs); 
      cout << "BAD and L are not intersect on a point..." << endl;
    }
  }
  else {
    s3fpWriteOutputs<OFT>(outfile, 6, dummy_errs); 
    cout << "BAD and L are not intersect at all..." << endl;
  }

  OFT n_sig = 1; 
  fwrite(&n_sig, sizeof(OFT), 1, outfile); 

  fclose(outfile); 
  outfile = fopen(argv[argc-1], "r"); 
  
  unsigned long ofsize = s3fpFileSize(outfile);     
  assert(ofsize % sizeof(OFT) == 0); 
  ofsize = ofsize / sizeof(OFT); 
  assert(ofsize == 14); 
  
  fseek(outfile, sizeof(OFT), SEEK_SET); 
  
  OFT detABL, detBCL, detCAL; 
  OFT detBAL, detADL, detDBL; 
  
  fread(&detABL, sizeof(OFT), 1, outfile); 
  fread(&detBCL, sizeof(OFT), 1, outfile); 
  fread(&detCAL, sizeof(OFT), 1, outfile); 
  
  fseek(outfile, sizeof(OFT)*3, SEEK_CUR); 
  
  fread(&detBAL, sizeof(OFT), 1, outfile); 
  fread(&detADL, sizeof(OFT), 1, outfile); 
  fread(&detDBL, sizeof(OFT), 1, outfile); 

  fseek(outfile, sizeof(OFT)*3, SEEK_CUR); 
  
  fseek(outfile, sizeof(OFT), SEEK_CUR); 
  
  bool inABC = (detABL <= 0 && detBCL <= 0 && detCAL <= 0);
  bool inBAD = (detBAL <= 0 && detADL <= 0 && detDBL <= 0); 
    
  OFT osig = 0.0; 
  
  if (vmethod == 0) {
    if (inABC && inBAD) {
      osig = 3.0; 
    }
    else if (inABC && !inBAD) {
      osig = 1.0; 
    }
    else if (!inABC && inBAD) {
      osig = 2.0; 
    }
    else if (detABL > 0 && detBCL < 0 && detCAL < 0 && 
	     detBAL > 0 && detADL < 0 && detDBL < 0) { 
      osig = 0.0; 
    }
    else {
      osig = 4.0; 
    }
  }
  else assert(false && "Error: not accepted vmethod..."); 

  fclose(outfile); 
  
  outfile = fopen(argv[argc-1], "a"); 
  fwrite(&osig, sizeof(OFT), 1, outfile); 
    
  fclose(infile); 
  fclose(outfile);

  return 0; 
}
