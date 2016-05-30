
# binary targets 
bin_PROGRAMS = rigidBody1_32 rigidBody1_64 rigidBody1_128 

# ==== examples ====

rigidBody1_32_SOURCES  = rigidBody1.c 
rigidBody1_32_CPPFLAGS = -DFT=float 
rigidBody1_32_LDFLAGS  = 
rigidBody1_32_LDADD    = -lquadmath 

rigidBody1_64_SOURCES  = rigidBody1.c 
rigidBody1_64_CPPFLAGS = -DFT=double
rigidBody1_64_LDFLAGS  = 
rigidBody1_64_LDADD    = -lquadmath 

rigidBody1_128_SOURCES  = rigidBody1.c 
rigidBody1_128_CPPFLAGS = -DFT=__float128 
rigidBody1_128_LDFLAGS  = 
rigidBody1_128_LDADD    = -lquadmath 

