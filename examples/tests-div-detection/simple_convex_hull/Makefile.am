
# binary targets 
bin_PROGRAMS = simple_convex_hull_32 simple_convex_hull_64 simple_convex_hull_128 

# ==== examples ====

simple_convex_hull_32_SOURCES = simple_convex_hull.cpp 
simple_convex_hull_32_CPPFLAGS = -DWFT=float -I../../include 
simple_convex_hull_32_LDFLAGS = 
simple_convex_hull_32_LDADD = -lquadmath 

simple_convex_hull_64_SOURCES = simple_convex_hull.cpp 
simple_convex_hull_64_CPPFLAGS = -DWFT=double -I../../include 
simple_convex_hull_64_LDFLAGS = 
simple_convex_hull_64_LDADD = -lquadmath 

simple_convex_hull_128_SOURCES = simple_convex_hull.cpp 
simple_convex_hull_128_CPPFLAGS = -DWFT=__float128 -I../../include 
simple_convex_hull_128_LDFLAGS = 
simple_convex_hull_128_LDADD = -lquadmath 