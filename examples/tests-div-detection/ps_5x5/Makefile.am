
# binary targets 
bin_PROGRAMS = ps_5x5_32 ps_5x5_64 ps_5x5_128 

# ==== examples ====

ps_5x5_32_SOURCES = ps_5x5.cpp 
ps_5x5_32_CPPFLAGS = -DWFT=float -I../../include -I../../lib/div_geometric_primitives 
ps_5x5_32_LDFLAGS = -L../../lib/div_geometric_primitives 
ps_5x5_32_LDADD = -ldiv_geometric_primitives -lquadmath 

ps_5x5_64_SOURCES = ps_5x5.cpp 
ps_5x5_64_CPPFLAGS = -DWFT=double -I../../include -I../../lib/div_geometric_primitives 
ps_5x5_64_LDFLAGS = -L../../lib/div_geometric_primitives 
ps_5x5_64_LDADD = -ldiv_geometric_primitives -lquadmath 

ps_5x5_128_SOURCES = ps_5x5.cpp 
ps_5x5_128_CPPFLAGS = -DWFT=__float128 -I../../include -I../../lib/div_geometric_primitives 
ps_5x5_128_LDFLAGS = -L../../lib/div_geometric_primitives 
ps_5x5_128_LDADD = -ldiv_geometric_primitives -lquadmath 