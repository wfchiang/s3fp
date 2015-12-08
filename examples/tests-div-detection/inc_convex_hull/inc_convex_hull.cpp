#include <iostream> 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
extern "C" {
#include "quadmath.h"
}
#include "s3fp_utils.h"

using namespace std; 

#ifndef IFT
#define IFT float
#endif 

#ifndef OFT 
#define OFT __float128 
#endif 

long N = 0; 
bool RAISE_ASSERTION = false; 
int CANO_FORM = 0; 
int SAMPLE_PHASE = 0; 
int VERIFY_METHOD = 0; 

struct Point {
  long id; 
  IFT x; 
  IFT y; 
};

vector<Point> PointList; 
vector<Point> Convexhull; 
vector<Point> Insiders; 

/* 
   test the orientation of point r wrt to line p -> q
   return 1 for p -> q -> r is counter clock-wise
   return 0 for p -> q -> r is collinear 
   return -1 for p -> q -> r is clock-wise 

   if outfile == NULL, the determinant will not be recorded 
*/
template<typename OWFT> 
int Orientation (Point p, Point q, Point r, OWFT &ret_det) {
  OWFT px = (OWFT) p.x; 
  OWFT py = (OWFT) p.y;
  OWFT qx = (OWFT) q.x;
  OWFT qy = (OWFT) q.y;
  OWFT rx = (OWFT) r.x;
  OWFT ry = (OWFT) r.y; 

  OWFT det = ((qx - px) * (ry - py)) - ((qy - py) * (rx - px));	

  ret_det = det; 

  if (det > 0) return 1;
  else if (det < 0) return -1;
  else return 0; 
}

void InsertPV (vector<Point>& pv, long index, Point p) {
  assert(index <= pv.size()); 
  if (index == pv.size()) pv.push_back(p); 
  else {
    vector<Point>::iterator pi = pv.begin(); 
    pi += index; 
    pv.insert(pi, p); 
  }
}

void DeletePV (vector<Point>& pv, long sindex, long eindex) {
  assert(sindex < eindex); 
  assert(pv.size() >= (eindex - sindex)); 
  
  vector<Point>::iterator hi = pv.begin() + sindex; 
  pv.erase((pv.begin() + sindex), (pv.begin() + eindex));
}


/*
  case cano_selection = 1:
  
  sort by point id 

  ----

  case cano_selection = 2:
  
  sort by point's geometric position
*/
void Canonical_Unordered_Points  (vector<Point>& pv, int cano_selection) {
  assert(1 <= cano_selection && cano_selection <= 2); 
  
  if (pv.size() == 0 || pv.size() == 1) return; 

  vector<Point> temp_pv; 
  int *new_inds = (int*) malloc(sizeof(int) * pv.size()); 

  for (int i = 0 ; i < pv.size() ; i++) 
    new_inds[i] = i; 

  for (int i = 0 ; i < pv.size() ; i++) {
    for (int j = 0 ; j < (pv.size() - 1) ; j++) {
      int this_ind = new_inds[j]; 
      int next_ind = new_inds[j+1]; 
      assert(pv[this_ind].id != pv[next_ind].id); 
      bool go_swap = false; 

      if (cano_selection == 1) {
	if (pv[this_ind].id > pv[next_ind].id) go_swap = true; 
      }
      else if (cano_selection == 2) {
	if (pv[this_ind].x < pv[next_ind].x) go_swap = false; 
	else if (pv[this_ind].x == pv[next_ind].x) {
	  if (pv[this_ind].y <= pv[next_ind].y) go_swap = false; 
	  else go_swap = true; 
	}
	else go_swap = true; 
      }
      else assert(false); 

      if (go_swap) {
	int tempi = new_inds[j];
	new_inds[j] = new_inds[j+1];
	new_inds[j+1] = tempi; 
      }
    }
  }

  for (int i = 0 ; i < pv.size() ; i++) 
    temp_pv.push_back(pv[new_inds[i]]); 
  pv.clear();
  pv.insert(pv.begin(), temp_pv.begin(), temp_pv.end()); 
}


/*
  case: cano_selection = 1

  Let the new head point has id h_id. The two adjacent points has ids, left_id and right_id. 

  h_id is the smallest id among all points. 
  Also, the list is rotated by direction h_id -> left_id if left_id < right_id.
  Otherwise, the list is rotated by direction h_id -> right_id if right_id < left_id. 

  ----

  case cano_selection = 2 
  
  Let the new head point has the hightest y coordinate and is the left-most. 

  The new list is in an counter clock-wise order 

*/ 
template <typename OWFT> 
void Canonical_Ordered_Points (vector<Point>& pv, int cano_selection) {
  assert(1 <= cano_selection && cano_selection <= 2); 
  assert(pv.size() >= 3); 

  int nh_index = 0; 
  vector<Point> temp_pv; 
  
  // find the new head 
  for (int i = 1 ; i < pv.size() ; i++) {
    assert(pv[nh_index].id != pv[i].id); 
    
    if (cano_selection == 1) {
      if (pv[nh_index].id > pv[i].id) nh_index = i; 
    }
    else if (cano_selection == 2) {
      if (pv[nh_index].y < pv[i].y) nh_index = i; 
      else if (pv[nh_index].y == pv[i].y) {
	if (pv[nh_index].x > pv[i].x) nh_index = i; 
	else ; 
      }
      else ; 
    }
    else assert(false); 
  }
  
  // find left and right indexes 
  int left_index = ((nh_index == 0) ? (pv.size() - 1) : (nh_index - 1)); 
  int right_index = (nh_index + 1) % pv.size(); 
  assert(0 <= left_index && left_index < pv.size()); 
  assert(0 <= right_index && right_index < pv.size()); 

  // decide rotation direciton
  bool to_left; 
  if (cano_selection == 1) {
    int left_id = pv[left_index].id; 
    int right_id = pv[right_index].id; 
    assert(pv[nh_index].id != left_id); 
    assert(pv[nh_index].id != right_id); 
    assert(left_id != right_id); 

    if (left_id < right_id) to_left = true;
    else to_left = false;    

    // this is hacking! 
    to_left = false; 
  }
  else if (cano_selection == 2) {
    OWFT ret_det; 
    int ori = Orientation<OWFT> (pv[right_index], 
				 pv[nh_index], 
				 pv[left_index], 
				 ret_det); 
    if (ori == 1) to_left = true;  
    else if (ori == 0) {
      OWFT inner = 
	((((OWFT)pv[nh_index].x) - ((OWFT)pv[right_index].x)) * 
	 (((OWFT)pv[left_index].x) - ((OWFT)pv[right_index].x))) +
	((((OWFT)pv[nh_index].y) - ((OWFT)pv[right_index].y)) *
	 (((OWFT)pv[left_index].y) - ((OWFT)pv[right_index].y))); 
      if (inner >= 0) to_left = true;
      else to_left = false; 
    }
    else if (ori == -1) to_left = false; 
    else assert(false); 
  }
  else assert(false); 

  // reinstall the point list 
  temp_pv.push_back(pv[nh_index]);
  for (int i = 1 ; i < pv.size() ; i++) {
    int next_index; 
    if (to_left) {
      next_index = nh_index - i; 
      if (next_index < 0) next_index += pv.size(); 
    }
    else next_index = (nh_index + i) % pv.size(); 
    
    temp_pv.push_back(pv[next_index]);
  }
  
  pv.clear(); 
  pv.insert(pv.begin(), temp_pv.begin(), temp_pv.end()); 
}


void PrintPoint(Point p) {
  cout << "[" << p.id << "](" << (long double) p.x << ", " << (long double) p.y << ")"; 
}

void PrintPV (vector<Point> pv) {
  vector<Point>::iterator pi = pv.begin();
  for ( ; pi != pv.end() ; pi++) {
    PrintPoint((*pi));
    cout << endl;
  }
}

void WRONG_CONVEX_HULL (FILE *outfile) {
  assert(outfile != NULL); 

  OFT odata = 1; 
  fwrite(&odata, sizeof(OFT), 1, outfile);
  fwrite(&odata, sizeof(OFT), 1, outfile);

  fwrite(&odata, sizeof(OFT), 1, outfile);
  odata = -1; 
  fwrite(&odata, sizeof(OFT), 1, outfile);

  RAISE_ASSERTION = true; 
}


void ReadInputs (FILE *infile) {
  assert(infile != NULL); 
  IFT idatax; 
  IFT idatay; 
  
  fseek(infile, 0, SEEK_END); 
  long nbytes = ftell(infile); 
  assert(nbytes % (sizeof(IFT) * 2) == 0);
  N = nbytes / (sizeof(IFT) * 2); 
  assert(N >= 3); 
  
  fseek(infile, 0, SEEK_SET);
  for (long i = 0 ; i < N ; i++) {
    fread(&idatax, sizeof(IFT), 1, infile); 
    fread(&idatay, sizeof(IFT), 1, infile); 
    struct Point newPoint; 
    newPoint.id = i; 
    newPoint.x = (IFT) idatax; 
    newPoint.y = (IFT) idatay; 
    PointList.push_back(newPoint); 
  }
}


void ComputeConvexhull (FILE *outfile) { 
  // find the first hull (triangle) 
  assert(PointList.size() >= 3); 
  Convexhull.push_back(PointList[0]); 
  Convexhull.push_back(PointList[1]); 
  vector<Point>::iterator pi = PointList.begin(); 
  PointList.erase(pi, pi+2); 
  pi = PointList.begin(); 
  for ( ; 
       pi != PointList.end() ; 
       pi++) {

    WFT det; 
    int ori = Orientation<WFT>(Convexhull[0], Convexhull[1], (*pi), det); 
    
    if (ori > 0) {
      Convexhull.push_back((*pi));
      PointList.erase(pi); 
      break;
    }
    else if (ori < 0) {
      InsertPV(Convexhull, 1, (*pi));
      PointList.erase(pi);
      break;
    }
    else continue; 
  }
#ifdef __VERBOSE
  PrintPV(Convexhull); 
#endif
  assert(Convexhull.size() == 3); 

  // traverse over PointList 
  pi = PointList.begin();
  for( ; pi != PointList.end() ; pi++) {
    assert(Convexhull.size() >= 3); 
#ifdef __VERBOSE
    cout << "Insert Point: "; 
    PrintPoint((*pi)); 
    cout << endl;
#endif 
    long vis_to_inv = -1; 
    long inv_to_vis = -1; 

    for (long hi = 0 ; hi < Convexhull.size() ; hi++) {
      long prev_hi = ((hi == 0) ? (Convexhull.size()-1) : (hi-1));
      long this_hi = hi;
      long next_hi = (hi + 1) % Convexhull.size(); 

      WFT det_prev_this; 
      WFT det_this_next; 

      int ori_prev_this = Orientation<WFT>(Convexhull[prev_hi], 
					   Convexhull[this_hi], 
					   (*pi), 
					   det_prev_this);
      int ori_this_next = Orientation<WFT>(Convexhull[this_hi], 
					   Convexhull[next_hi], 
					   (*pi), 
					   det_this_next);     

      if (ori_prev_this <= 0 && 
	  ori_this_next > 0) { // point (*pi) can see prev -> this but cannot see this -> next. 

	if (vis_to_inv != -1) { // a vis_to_inv had been detected -> impossible!! -> so error found!! 
#ifdef __VERBOSE
	  cout << "curr point: "; PrintPoint((*pi)); cout << endl;
	  cout << "prev vis_to_inv: " << vis_to_inv << endl;
	  cout << "hi: " << hi << endl;
	  cout << "Hull[prev_hi]: "; PrintPoint(Convexhull[prev_hi]); cout << endl;
	  cout << "Hull[this_hi]: "; PrintPoint(Convexhull[this_hi]); cout << endl;
	  cout << "Hull[next_hi]: "; PrintPoint(Convexhull[next_hi]); cout << endl;
#endif 
	  WRONG_CONVEX_HULL(outfile); 
	  return; 
	}

	vis_to_inv = hi; 
      }
      else if (ori_prev_this > 0 && 
	       ori_this_next <= 0) { // point (* pi) can cannot see prev -> this but can see this -> next 

	if (inv_to_vis != -1) { // a inv_to_vis had been detected -> impossible! -> so error found!! 
#ifdef __VERBOSE
	  cout << "curr point: "; PrintPoint((*pi)); cout << endl;
	  cout << "prev inv_to_vis: " << inv_to_vis << endl;
	  cout << "hi: " << hi << endl;
	  cout << "Hull[prev_hi]: "; PrintPoint(Convexhull[prev_hi]); cout << endl;
	  cout << "Hull[this_hi]: "; PrintPoint(Convexhull[this_hi]); cout << endl;
	  cout << "Hull[next_hi]: "; PrintPoint(Convexhull[next_hi]); cout << endl;
#endif 
	  WRONG_CONVEX_HULL(outfile); 
	  return ;
	}

	inv_to_vis = hi; 
      }
      else ;
    } 
    if (inv_to_vis == -1 && 
	vis_to_inv == -1) 
      Insiders.push_back((*pi));
    else {
      if (((inv_to_vis == -1) && (vis_to_inv != -1)) || 
	  ((inv_to_vis != -1) && (vis_to_inv == -1))) {
#ifdef __VERBOSE
	cout << "ERROR!" << endl;
	cout << "Point " << endl; PrintPoint((*pi)); cout << endl;
	cout << "inv_to_vis: " << inv_to_vis << endl;
	cout << "vis_to_inv: " << vis_to_inv << endl;
#endif 
	WRONG_CONVEX_HULL(outfile);
	return ;
      }

      assert(inv_to_vis != vis_to_inv); 
      
      if (inv_to_vis > vis_to_inv) {
	if (Convexhull.size() - inv_to_vis > 1) {
	  for (long hi = inv_to_vis+1 ; hi < Convexhull.size() ; hi++)
	    Insiders.push_back(Convexhull[hi]);
	  DeletePV(Convexhull, inv_to_vis+1, Convexhull.size()); 
	}
	if (vis_to_inv > 0) {
	  for (long hi = 0 ; hi < vis_to_inv ; hi++)
	    Insiders.push_back(Convexhull[hi]);
	  DeletePV(Convexhull, 0, vis_to_inv); 
	}
	InsertPV(Convexhull, 0, (*pi));
      }
      else if (inv_to_vis < vis_to_inv) {
	if ((vis_to_inv - inv_to_vis) > 1) {
	  for (long hi = inv_to_vis+1 ; hi < vis_to_inv ; hi++)
	    Insiders.push_back(Convexhull[hi]);
	  DeletePV(Convexhull, inv_to_vis+1, vis_to_inv);
	}
	InsertPV(Convexhull, inv_to_vis+1, (*pi));
      }
      else assert(false); 
    }
#ifdef __VERBOSE
    PrintPV(Convexhull);
#endif  
  }

  // print final convex hull
#ifdef __VERBOSE
  cout << "==== final convex hull ====" << endl;
  PrintPV(Convexhull); 
  cout << "=====================" << endl;
#endif 
}


void VerifyHull(FILE *outfile) {
  assert(Insiders.size() + Convexhull.size() == N); 
  assert(Convexhull.size() >= 3);

  if (VERIFY_METHOD == 0) return; 

  else if (VERIFY_METHOD == 1) { 
    if (CANO_FORM > 0) {
      Canonical_Ordered_Points<WFT>(Convexhull, 
				    CANO_FORM); 
      Canonical_Unordered_Points(Insiders, 
				 CANO_FORM); 
    }
    
#ifdef __VERBOSE
    cout << "---- points inside hull ----" << endl;
    PrintPV(Insiders); 
    cout << "---- ordered convex hull ----" << endl;
    PrintPV(Convexhull); 
#endif
    
    int this_ori = 0; 
    int ideal_ori = 0; 
    
    long ii; 
    for (ii = 0 ; ii < Convexhull.size() - 2 ; ii++) {
      WFT det; 
      ideal_ori = Orientation<WFT>(Convexhull[ii], Convexhull[ii+1], Convexhull[ii+2], det); 
      if (ideal_ori == 0) continue;
      else break; 
    }
    assert(ii < Convexhull.size() - 2); 
    
    vector<Point>::iterator pi = Insiders.begin(); 
    for ( ; pi != Insiders.end() ; pi++) {
      long hi_p = 0; 
      long hi_q = 1; 
      
      for ( ; hi_p < Convexhull.size() ; hi_p++) {
	hi_q = (hi_p + 1) % Convexhull.size(); 
	
	if (SAMPLE_PHASE == 1) {
	  WFT det; 
	  this_ori = Orientation<WFT>(Convexhull[hi_p], Convexhull[hi_q], (*pi), det); 
	  if (this_ori != 0) {
	    if (this_ori * ideal_ori < 0) {
	      WRONG_CONVEX_HULL(outfile);
	      return;
	    }
	  }
	}
      }
    }
  }
  
  else assert(false && "Error: Invalid verification method"); 
}

int main (int argc, char *argv[]) {
  assert(argc == 6);

  CANO_FORM = atoi(argv[1]); 
  assert(0 <= CANO_FORM && CANO_FORM <= 2); 
  
  SAMPLE_PHASE = atoi(argv[2]); 
  assert(0 <= SAMPLE_PHASE && SAMPLE_PHASE <= 1); 

  VERIFY_METHOD = atoi(argv[3]); 
  assert(0 <= VERIFY_METHOD && VERIFY_METHOD <= 1); 
	 
  FILE *infile = s3fpGetInFile(argc, argv); 
  FILE *outfile = s3fpGetOutFile(argc, argv); 

  ReadInputs(infile); 

  ComputeConvexhull(outfile); 

  // VerifyHull(outfile); // VerifyHull will close the outfile 

  if (RAISE_ASSERTION == false) {
    OFT odata = 1; 
    fwrite(&odata, sizeof(OFT), 1, outfile); 
    fwrite(&odata, sizeof(OFT), 1, outfile); 

    fwrite(&odata, sizeof(OFT), 1, outfile); 
    odata = Convexhull.size();
    fwrite(&odata, sizeof(OFT), 1, outfile); 
  }
  
  fclose(infile); 
  fclose(outfile); 

  return 0; 
}
