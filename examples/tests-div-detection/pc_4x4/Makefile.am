
# binary targets 
bin_PROGRAMS = pc_4x4_32 pc_4x4_64 pc_4x4_128 

# ==== examples ====

pc_4x4_32_SOURCES = pc_4x4.cpp 
pc_4x4_32_CPPFLAGS = -DWFT=float -I../../include -I../../lib/div_geometric_primitives 
pc_4x4_32_LDFLAGS = -L../../lib/div_geometric_primitives 
pc_4x4_32_LDADD = -ldiv_geometric_primitives -lquadmath 

pc_4x4_64_SOURCES = pc_4x4.cpp 
pc_4x4_64_CPPFLAGS = -DWFT=double -I../../include -I../../lib/div_geometric_primitives 
pc_4x4_64_LDFLAGS = -L../../lib/div_geometric_primitives 
pc_4x4_64_LDADD = -ldiv_geometric_primitives -lquadmath 

pc_4x4_128_SOURCES = pc_4x4.cpp 
pc_4x4_128_CPPFLAGS = -DWFT=__float128 -I../../include -I../../lib/div_geometric_primitives 
pc_4x4_128_LDFLAGS = -L../../lib/div_geometric_primitives 
pc_4x4_128_LDADD = -ldiv_geometric_primitives -lquadmath 