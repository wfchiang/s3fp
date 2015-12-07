
# binary targets 
bin_PROGRAMS = pp_3x3_32 pp_3x3_64 pp_3x3_128 

# ==== examples ====

pp_3x3_32_SOURCES = pp_3x3.cpp 
pp_3x3_32_CPPFLAGS = -DWFT=float -I../../include -I../../lib/div_geometric_primitives 
pp_3x3_32_LDFLAGS = -L../../lib/div_geometric_primitives 
pp_3x3_32_LDADD = -ldiv_geometric_primitives -lquadmath 

pp_3x3_64_SOURCES = pp_3x3.cpp 
pp_3x3_64_CPPFLAGS = -DWFT=double -I../../include -I../../lib/div_geometric_primitives 
pp_3x3_64_LDFLAGS = -L../../lib/div_geometric_primitives 
pp_3x3_64_LDADD = -ldiv_geometric_primitives -lquadmath 

pp_3x3_128_SOURCES = pp_3x3.cpp 
pp_3x3_128_CPPFLAGS = -DWFT=__float128 -I../../include -I../../lib/div_geometric_primitives 
pp_3x3_128_LDFLAGS = -L../../lib/div_geometric_primitives 
pp_3x3_128_LDADD = -ldiv_geometric_primitives -lquadmath 