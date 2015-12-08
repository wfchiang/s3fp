
# binary targets 
bin_PROGRAMS = pp_4x4_32 pp_4x4_64 pp_4x4_128 

# ==== examples ====

pp_4x4_32_SOURCES = pp_4x4.cpp 
pp_4x4_32_CPPFLAGS = -DWFT=float -I../../include -I../../lib/div_geometric_primitives 
pp_4x4_32_LDFLAGS = -L../../lib/div_geometric_primitives 
pp_4x4_32_LDADD = -ldiv_geometric_primitives -lquadmath 

pp_4x4_64_SOURCES = pp_4x4.cpp 
pp_4x4_64_CPPFLAGS = -DWFT=double -I../../include -I../../lib/div_geometric_primitives 
pp_4x4_64_LDFLAGS = -L../../lib/div_geometric_primitives 
pp_4x4_64_LDADD = -ldiv_geometric_primitives -lquadmath 

pp_4x4_128_SOURCES = pp_4x4.cpp 
pp_4x4_128_CPPFLAGS = -DWFT=__float128 -I../../include -I../../lib/div_geometric_primitives 
pp_4x4_128_LDFLAGS = -L../../lib/div_geometric_primitives 
pp_4x4_128_LDADD = -ldiv_geometric_primitives -lquadmath 