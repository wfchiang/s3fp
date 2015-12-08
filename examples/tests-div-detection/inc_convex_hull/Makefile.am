
# binary targets 
bin_PROGRAMS = inc_convex_hull_32 inc_convex_hull_64 inc_convex_hull_128 

# ==== examples ====

inc_convex_hull_32_SOURCES = inc_convex_hull.cpp 
inc_convex_hull_32_CPPFLAGS = -DWFT=float -I../../include 
inc_convex_hull_32_LDFLAGS = 
inc_convex_hull_32_LDADD = -lquadmath 

inc_convex_hull_64_SOURCES = inc_convex_hull.cpp 
inc_convex_hull_64_CPPFLAGS = -DWFT=double -I../../include 
inc_convex_hull_64_LDFLAGS = 
inc_convex_hull_64_LDADD = -lquadmath 

inc_convex_hull_128_SOURCES = inc_convex_hull.cpp 
inc_convex_hull_128_CPPFLAGS = -DWFT=__float128 -I../../include 
inc_convex_hull_128_LDFLAGS = 
inc_convex_hull_128_LDADD = -lquadmath 