
#include "cgal_precision_selection.h"

#include <CGAL/convex_hull_2.h>

#include <iostream>
#include <assert.h>

/* 
   Algorithm selection: 
   [0]: system's choice 
   [1]: Bykat's algorithm 
   [2]: Akl and Toussaint's algorithm 
   [3]: Graham and Andrew's algorithm 
*/


void 
CGAL_Convexhull (int algo_selection, 
		 std::vector<Point_2> pvec, 
		 std::vector<Point_2>& ret, 
		 bool verb=false, 
		 FILE *errfile=NULL) {
  // check algo_selection 
  assert(0 <= algo_selection && algo_selection <= 3); 

  // save previous settings and install current setting 
  bool prev_sdet_verb; 
  bool prev_disc_verb; 
  bool prev_orient_verb; 
  
  bool prev_log_error; 
  FILE *prev_errfile; 

  prev_sdet_verb = WF_SDET_VERBOSE; 
  prev_disc_verb = WF_DISC_VERBOSE; 
  prev_orient_verb = WF_ORIENT_VERBOSE; 

  prev_log_error = WF_ORIENT_LOG_ERROR; 
  prev_errfile = WF_ORIENT_ERRFILE; 
  
  WF_SDET_VERBOSE = verb; 
  WF_DISC_VERBOSE = verb; 
  WF_ORIENT_VERBOSE = verb; 
  
  if (errfile == NULL) 
    WF_ORIENT_LOG_ERROR = false;
  else {
    WF_ORIENT_LOG_ERROR = true; 
    WF_ORIENT_ERRFILE = errfile; 
  }
  
  // run convexhull 
  switch (algo_selection) {
  case 0: // system's choice 
    CGAL::convex_hull_2 (pvec.begin(), 
			 pvec.end(), 
			 std::back_inserter(ret));
    break;
  case 1: // Bykat 
    CGAL::ch_bykat (pvec.begin(), 
		    pvec.end(), 
		    std::back_inserter(ret));
    break;
  case 2: // Akl and Toussaint 
    CGAL::ch_akl_toussaint (pvec.begin(), 
			    pvec.end(), 
			    std::back_inserter(ret));
    break;
  case 3: // Graham and Andrew 
    CGAL::ch_graham_andrew (pvec.begin(), 
			    pvec.end(), 
			    std::back_inserter(ret));
    break; 
  default:
    assert(false); 
    break;
  }

  // restore previous settings 
  WF_SDET_VERBOSE = prev_sdet_verb; 
  WF_DISC_VERBOSE = prev_disc_verb; 
  WF_ORIENT_VERBOSE = prev_orient_verb; 

  WF_ORIENT_LOG_ERROR = prev_log_error; 
  WF_ORIENT_ERRFILE = prev_errfile; 
}


/* 
   get the reference position in the reference vector
   return -1 for fail...
*/
template<typename VPT> 
int 
GetReferenceId (std::vector<VPT> ref, 
		VPT p) {
  if (ref.size() <= 0) return -1; 
  for (int i = 0 ; i < ref.size() ; i++) {
    if(p == ref[i]) return i; 
  }
  return -1; 
}
		

/* 
   cano_selection
   [0]: no canonical form 
   [1]: by reference (id)
   [2]: by geometry
*/ 
template<typename VPT> 
void 
Canonical_Convexhull (std::vector<VPT> ext_pvec, 
		      std::vector<VPT> ext_conv, 
		      std::vector<VPT>& cano_ext_conv, 
		      int cano_selection) {
  assert(0 <= cano_selection && cano_selection <= 2); 
  assert(cano_ext_conv.size() == 0); 
  
  if (ext_conv.size() == 0) return ; 
  else if (ext_conv.size() == 1) {
    cano_ext_conv.push_back(ext_conv[0]); 
    return ; 
  }
  if (cano_selection == 0) {
    for (int ci = 0 ; ci < ext_conv.size() ; ci++) 
      cano_ext_conv.push_back(ext_conv[ci]);
    return ;
  }
  else if (cano_selection == 1) {
    int first_ind = -1; 
    int first_pos = ext_pvec.size(); 

    for (int ci = 0 ; ci < ext_conv.size() ; ci++) {
      int my_pos = GetReferenceId(ext_pvec, ext_conv[ci]); 
      assert(0 <= my_pos && my_pos < ext_pvec.size()); 
      assert(my_pos != first_pos); 
      if (my_pos < first_pos) {
	first_pos = my_pos; 
	first_ind = ci; 
      }
    }
    assert(0 <= first_ind && first_ind < ext_conv.size()); 
			      
    int left_ind = ((first_ind == 0) ? (ext_conv.size() - 1) : (first_ind - 1)); 
    assert(0 <= left_ind && left_ind < ext_conv.size()); 
    int left_pos = GetReferenceId(ext_pvec, ext_conv[left_ind]); 
    assert(0 <= left_pos && left_pos < ext_pvec.size()); 

    int right_ind = (first_ind + 1) % ext_conv.size(); 
    assert(0 <= right_ind && right_ind < ext_conv.size()); 
    int right_pos = GetReferenceId(ext_pvec, ext_conv[right_ind]); 
    assert(0 <= right_pos && right_pos < ext_pvec.size()); 

    assert(left_ind != right_ind); 
    assert(first_ind != left_ind); 
    assert(first_ind != right_ind);
    
    bool to_left = (left_pos < right_pos); 
    
    // this is a hacking! 
    to_left = false; 

    cano_ext_conv.push_back(ext_conv[first_ind]); 
    for (int ci = 1 ; ci < ext_conv.size() ; ci++) {
      int next_ind; 
      if (to_left) {
	next_ind = first_ind - ci; 
	if (next_ind < 0) next_ind += ext_conv.size(); 
      }
      else next_ind = (first_ind + ci) % ext_conv.size(); 
      cano_ext_conv.push_back(ext_conv[next_ind]); 
    }
  }
  else if (cano_selection == 2) {
    // find the top point 
    int id_bp = 0; 
    for (int i = 1 ; i < ext_conv.size() ; i++) 
      if (ext_conv[id_bp].y() > ext_conv[i].y()) id_bp = i; 

    if (ext_conv.size() == 2) {
      cano_ext_conv.push_back(ext_conv[id_bp]);
      cano_ext_conv.push_back(ext_conv[(id_bp+1)%2]); 
      return ; 
    }

    // ext_conv.size () >= 3 
    int lefti = ((id_bp == 0) ? (ext_conv.size() - 1) : (id_bp-1)); 
    int righti = (id_bp + 1) % ext_conv.size(); 
    assert(lefti != id_bp); 
    assert(righti != id_bp);
    assert(lefti != righti); 

    int id_ccw = -1; 
    int sdet = CGAL::sign_of_determinant ( (ext_conv[lefti].x() - ext_conv[righti].x()), 
					   (ext_conv[lefti].y() - ext_conv[righti].y()), 
					   (ext_conv[id_bp].x() - ext_conv[righti].x()), 
					   (ext_conv[id_bp].y() - ext_conv[righti].y()) ); 
    if (sdet == 0) {
      if (ext_conv[lefti].x() < ext_conv[righti].x()) id_ccw = lefti;
      else if (ext_conv[lefti].x() > ext_conv[righti].x()) id_ccw = righti;
      else {
	if (ext_conv[lefti].y() < ext_conv[righti].y()) id_ccw = lefti;
	else if (ext_conv[lefti].y() > ext_conv[righti].y()) id_ccw = righti; 
	else assert(false && "Canonical_Convexhull: Invalid Convexhull..."); 
      }
    }
    else if (sdet > 0) id_ccw = righti; 
    else id_ccw = lefti; 
    assert(id_ccw == lefti || id_ccw == righti); 

    cano_ext_conv.push_back(ext_conv[id_bp]); 
    for (int i = 1 ; i < ext_conv.size() ; i++) {
      int id_next = ((id_ccw == lefti) ? (id_bp - i) : (id_bp + i)); 
      id_next = ((id_ccw == lefti) ? 
		 ((id_next >= 0) ? id_next : (id_next + ext_conv.size())) : 
		 (id_next % ext_conv.size())); 
      assert(0 <= id_next && id_next < ext_conv.size()); 
      cano_ext_conv.push_back(ext_conv[id_next]);
    }
  }
  else assert(false); 
}


template<typename VPT> 
void 
Canonical_Points (std::vector<VPT> ext_pvec, 
		  std::vector<VPT> ext_inside, 
		  std::vector<VPT>& cano_ext_inside, 
		  int cano_selection) {
  assert(0 <= cano_selection && cano_selection <= 2); 

  assert(cano_ext_inside.size() == 0); 
  if (ext_inside.size() == 0) return; 

  int *ids = (int *)malloc(sizeof(int) * ext_inside.size()); 
  for (int i = 0 ; i < ext_inside.size() ; i++) 
    ids[i] = i; 
   
  if (cano_selection == 0) {
    for (int i = 0 ; i < ext_inside.size() ; i++) 
      cano_ext_inside.push_back(ext_inside[i]); 
    return ;
  }
  else if (cano_selection == 1) {
    int *poss = (int *)malloc(sizeof(int) * ext_inside.size()); 
    for (int i = 0 ; i < ext_inside.size() ; i++)
      poss[i] = GetReferenceId(ext_pvec, ext_inside[i]); 

    for (int i = 0 ; i < ext_inside.size() ; i++) {
      for (int j = 0 ; j < ((ext_inside.size() - 1) - i) ; j++) {
	if (poss[ids[j]] <= poss[ids[j+1]]) continue;
	
	int tempi = ids[j];
	ids[j] = ids[j+1];
	ids[j+1] = tempi; 
      }
    }
  }
  else if (cano_selection == 2) {
    for (int i = 0 ; i < ext_inside.size() ; i++) {
      for (int j = 0 ; j < ((ext_inside.size() - 1) - i) ; j++) {
	if (ext_inside[ids[j]].x() < ext_inside[ids[j+1]].x()) continue;
	else if (ext_inside[ids[j]].x() == ext_inside[ids[j+1]].x()) 
	  if (ext_inside[ids[j]].y() <= ext_inside[ids[j+1]].y()) continue; 
	int tempi = ids[j]; 
	ids[j] = ids[j+1];
	ids[j+1] = tempi; 
      }
    }
  }
  else assert(false); 

  for (int i = 0 ; i < ext_inside.size() ; i++) 
    cano_ext_inside.push_back( ext_inside[ids[i]] ); 
}


/*
  verify_selection:
  [0]: system's method
  [1]: system assist 
  [2]: manual method 
  [3]: convex polygon check 
*/ 

template<typename VPT> 
bool 
CGAL_Verify_Convexhull (int verify_selection, 
			std::vector<Point_2> pvec, 
			std::vector<Point_2> conv, 
			int cano_selection=0, 
			bool verb=false, 
			FILE *errfile=NULL) {
  // check verify_selection
  assert(0 <= verify_selection && verify_selection <= 3); 

  // check canonical selection 
  assert(0 <= cano_selection && cano_selection <= 2); 

  // save previous settings 
  bool prev_sdet_verb = WF_SDET_VERBOSE; 
  bool prev_disc_verb = WF_DISC_VERBOSE; 
  bool prev_orient_verb = WF_ORIENT_VERBOSE; 

  bool prev_log_error = WF_ORIENT_LOG_ERROR; 
  FILE *prev_errfile = WF_ORIENT_ERRFILE; 
  
  // install current settings 
  WF_SDET_VERBOSE = verb; 
  WF_DISC_VERBOSE = verb; 
  WF_ORIENT_VERBOSE = verb; 

  if (errfile == NULL) 
    WF_ORIENT_LOG_ERROR = false; 
  else {
    WF_ORIENT_LOG_ERROR = true;
    WF_ORIENT_ERRFILE = errfile; 
  }

  // create the point set and the convex hull in high precision 
  typename std::vector<VPT> ext_pvec; 
  typename std::vector<VPT> ext_conv;
  typename std::vector<VPT> ext_inside; 
  std::vector<Point_2>::iterator pi; 
  std::vector<Point_2>::iterator ci; 
  typename std::vector<VPT>::iterator eci;
  typename std::vector<VPT>::iterator eii; 
  for (pi = pvec.begin() ; pi != pvec.end() ; pi++) {
    VPT ppoint (pi->x(), pi->y()); 
    ext_pvec.push_back(ppoint); 
  }
  for (ci = conv.begin() ; ci != conv.end() ; ci++) {
    VPT cpoint (ci->x(), ci->y());
    ext_conv.push_back(cpoint);
  }
  for (pi = pvec.begin() ; pi != pvec.end() ; pi++) {
    VPT ppoint (pi->x(), pi->y());
    for (eci = ext_conv.begin() ; eci != ext_conv.end() ; eci++) 
      if ((*eci) == ppoint) break;
    if (eci == ext_conv.end()) {
      ext_inside.push_back(ppoint); 
    }
  }
  assert(pvec.size() == ext_pvec.size()); 
  assert(conv.size() == ext_conv.size());
  assert(ext_pvec.size() >= (ext_conv.size() + ext_inside.size())); 

  // verify convexhull 
  bool verify_result = false; 
  int ii, ei, sdet; 
  int prev_sdet = 0; 
  vector<VPT> cano_ext_conv; 
  vector<VPT> cano_ext_inside; 
  switch (verify_selection) {
  case 0: // system's method 
    verify_result = (CGAL::is_ccw_strongly_convex_2(ext_conv.begin(), ext_conv.end()) || 
		     CGAL::is_cw_strongly_convex_2(ext_conv.begin(), ext_conv.end())); 
    break;
  case 1: // system assist 
    for (eii = ext_inside.begin() ; eii != ext_inside.end() ; eii++) {
      CGAL::Bounded_side bs = CGAL::bounded_side_2(ext_conv.begin(), ext_conv.end(), (*eii)); 
      if (bs == CGAL::ON_UNBOUNDED_SIDE) {
	cout << "Point: " << (*eii) << " is outside the convex hull!!" << endl;
	break;
      }
    }
    if (eii == ext_inside.end()) verify_result = true;
    else verify_result = false; 
    break; 
  case 2: 
    Canonical_Convexhull<VPT>(ext_pvec, ext_conv, cano_ext_conv, cano_selection); 
    /*
    cout << "-- old convexhull --" << endl;
    for (int i = 0 ; i < ext_conv.size() ; i++) 
      cout << ext_conv[i] << " : "; 
    cout << " " << endl;
    cout << "-- new convexhull --" << endl;
    for (int i = 0 ; i < cano_ext_conv.size() ; i++) 
      cout << cano_ext_conv[i] << " : ";
    cout << " " << endl;
    */
    Canonical_Points<VPT>(ext_pvec, ext_inside, cano_ext_inside, cano_selection); 
    /*
    cout << "-- old inside --" << endl;
    for (int i = 0 ; i < ext_inside.size() ; i++) 
      cout << ext_inside[i] << " : ";
    cout << " " << endl;
    cout << "-- new inside --" << endl;
    for (int i = 0 ; i < cano_ext_inside.size() ; i++) 
      cout << cano_ext_inside[i] << " : "; 
    cout << " " << endl;
    */
    verify_result = true; 

    for (ii = 0 ; ii < cano_ext_inside.size() ; ii++) {
      for (ei = 0 ; ei < cano_ext_conv.size() ; ei++) {
	/*
	sdet = CGAL::sign_of_determinant ( (cano_ext_conv[ei].x() - cano_ext_inside[ii].x()), 
					   (cano_ext_conv[ei].y() - cano_ext_inside[ii].y()), 
					   (cano_ext_conv[(ei+1)%cano_ext_conv.size()].x() - cano_ext_inside[ii].x()), 
					   (cano_ext_conv[(ei+1)%cano_ext_conv.size()].y() - cano_ext_inside[ii].y()) );
	*/

	sdet = CGAL::orientationC2( cano_ext_inside[ii].x(), cano_ext_inside[ii].y(), 
				    cano_ext_conv[ei].x(), cano_ext_conv[ei].y(), 
				    cano_ext_conv[(ei+1)%cano_ext_conv.size()].x(), cano_ext_conv[(ei+1)%cano_ext_conv.size()].y() ); 

	if (sdet != 0) {
	  if (prev_sdet == 0) prev_sdet = sdet; 
	  else if (prev_sdet != sdet) verify_result = false; 
	}
      }
    }
    break; 
  case 3: 
    Canonical_Convexhull<VPT>(ext_pvec, ext_conv, cano_ext_conv, cano_selection); 
    Canonical_Points<VPT>(ext_pvec, ext_inside, cano_ext_inside, cano_selection); 

    for (ei = 0 ; ei < cano_ext_conv.size() ; ei++) {
      sdet = CGAL::sign_of_determinant ( (cano_ext_conv[ei].x() - cano_ext_conv[(ei+2)%cano_ext_conv.size()].x()), 
					 (cano_ext_conv[ei].y() - cano_ext_conv[(ei+2)%cano_ext_conv.size()].y()), 
					 (cano_ext_conv[(ei+1)%cano_ext_conv.size()].x() - cano_ext_conv[(ei+2)%cano_ext_conv.size()].x()), 
					 (cano_ext_conv[(ei+1)%cano_ext_conv.size()].y() - cano_ext_conv[(ei+2)%cano_ext_conv.size()].y()) ); 
      if (sdet != 0) {
	if (prev_sdet == 0) prev_sdet = sdet; 
	else if (prev_sdet != sdet) verify_result = false; 
      }
    }
    break; 
  default:
    break; 
  }

  // restore previous settings 
  WF_SDET_VERBOSE = prev_sdet_verb; 
  WF_DISC_VERBOSE = prev_disc_verb; 
  WF_ORIENT_VERBOSE = prev_orient_verb; 
  
  WF_ORIENT_LOG_ERROR = prev_log_error;
  WF_ORIENT_ERRFILE = prev_errfile; 

  return verify_result; 
}
