
# binary targets 
bin_PROGRAMS = ps_4x4_32 ps_4x4_64 ps_4x4_128 

# ==== examples ====

ps_4x4_32_SOURCES = ps_4x4.cpp 
ps_4x4_32_CPPFLAGS = -DWFT=float -I../../include -I../../lib/div_geometric_primitives 
ps_4x4_32_LDFLAGS = -L../../lib/div_geometric_primitives 
ps_4x4_32_LDADD = -ldiv_geometric_primitives -lquadmath 

ps_4x4_64_SOURCES = ps_4x4.cpp 
ps_4x4_64_CPPFLAGS = -DWFT=double -I../../include -I../../lib/div_geometric_primitives 
ps_4x4_64_LDFLAGS = -L../../lib/div_geometric_primitives 
ps_4x4_64_LDADD = -ldiv_geometric_primitives -lquadmath 

ps_4x4_128_SOURCES = ps_4x4.cpp 
ps_4x4_128_CPPFLAGS = -DWFT=__float128 -I../../include -I../../lib/div_geometric_primitives 
ps_4x4_128_LDFLAGS = -L../../lib/div_geometric_primitives 
ps_4x4_128_LDADD = -ldiv_geometric_primitives -lquadmath 