
# binary targets 
bin_PROGRAMS = pc_3x3_32 pc_3x3_64 pc_3x3_128 

# ==== examples ====

pc_3x3_32_SOURCES = pc_3x3.cpp 
pc_3x3_32_CPPFLAGS = -DWFT=float -I../../include -I../../lib/div_geometric_primitives 
pc_3x3_32_LDFLAGS = -L../../lib/div_geometric_primitives 
pc_3x3_32_LDADD = -ldiv_geometric_primitives -lquadmath 

pc_3x3_64_SOURCES = pc_3x3.cpp 
pc_3x3_64_CPPFLAGS = -DWFT=double -I../../include -I../../lib/div_geometric_primitives 
pc_3x3_64_LDFLAGS = -L../../lib/div_geometric_primitives 
pc_3x3_64_LDADD = -ldiv_geometric_primitives -lquadmath 

pc_3x3_128_SOURCES = pc_3x3.cpp 
pc_3x3_128_CPPFLAGS = -DWFT=__float128 -I../../include -I../../lib/div_geometric_primitives 
pc_3x3_128_LDFLAGS = -L../../lib/div_geometric_primitives 
pc_3x3_128_LDADD = -ldiv_geometric_primitives -lquadmath 