
#include <CGAL/Cartesian.h>
// #include <CGAL/Exact_predicates_exact_constructions_kernel.h>
extern "C" {
#include "quadmath.h"
}

#ifdef CGALK32
typedef CGAL::Cartesian<float> Kernel; 
#define CGALWFT float
#endif 

#ifdef CGALK64
typedef CGAL::Cartesian<double> Kernel; 
#define CGALWFT double 
#endif 

#ifdef CGALKEXACT
typedef CGAL::Cartesian<CGAL::Gmpq> Kernel; 
// typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel; 
#define CGALWFT __float128 
#endif 

#include <CGAL/Sphere_3.h>
#include <CGAL/Polygon_2.h>

typedef Kernel::Point_2 Point_2;
typedef Kernel::Point_3 Point_3; 
typedef Kernel::Triangle_2 Triangle_2; 
typedef Kernel::Triangle_3 Triangle_3;
typedef Kernel::Circle_3 Circle_3; 
typedef Kernel::Sphere_3 Sphere_3; 
typedef Kernel::Plane_3 Plane_3; 

typedef Kernel::Segment_2 Segment_2; 

typedef Kernel::Line_2 Line_2; 
typedef Kernel::Line_3 Line_3; 

typedef CGAL::Polygon_2<Kernel> Polygon_2; 

typedef CGAL::Cartesian<CGAL::Gmpq> Kernel_Exact; 
// typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel_Exact; 
typedef Kernel_Exact::Point_2 Point_Exact_2; 
typedef Kernel_Exact::Line_2 Line_Exact_2; 
typedef CGAL::Polygon_2<Kernel_Exact> Polygon_Exact_2; 
