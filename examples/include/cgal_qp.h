
#include "cgal_precision_selection.h" 
#include "CGAL/QP_models.h"
#include "CGAL/QP_functions.h"


typedef CGAL::Quadratic_program_from_iterators 
< CGALWFT **, 
  CGALWFT *, 
  CGAL::Const_oneset_iterator<CGAL::Comparison_result>, 
  bool *, 
  CGALWFT *, 
  bool *, 
  CGALWFT *, 
  CGALWFT **, 
  CGALWFT *>
  Program; 


typedef CGAL::Quadratic_program_solution<CGALWFT> Solution; 

  
