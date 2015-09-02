
#include "cgal_precision_selection.h"

#include <CGAL/Boolean_set_operations_2.h>

#include <iostream> 
#include <list>
using namespace std; 

typedef CGAL::Polygon_with_holes_2<Kernel> PWH_2; 
typedef std::list<PWH_2> PWH_List; 


CGALWFT 
CGAL_SPolygon_Intersection_Area (Polygon_2 poly0, 
				 Polygon_2 poly1) {
  CGALWFT intersect_area = 0; 

  if (CGAL::do_intersect(poly0, poly1)) {

    PWH_List intersects; 
    PWH_List::const_iterator iit; 
    
    CGAL::intersection(poly0, poly1, 
		       std::back_inserter(intersects));

    for (iit = intersects.begin() ; iit != intersects.end() ; iit++) {
      CGALWFT out_area = CGAL::to_double(iit->outer_boundary().area());

      if (out_area < 0) out_area = out_area * -1; 

      PWH_2::Hole_const_iterator hit; 
      for (hit = iit->holes_begin() ; hit != iit->holes_end() ; hit++) {
	CGALWFT hole_area = CGAL::to_double(hit->area());
	if (hole_area < 0) hole_area = hole_area * -1; 
	out_area -= hole_area; 
      }

      intersect_area += out_area; 
    }

  }
  else intersect_area = 0.0; 

  return intersect_area; 
}
