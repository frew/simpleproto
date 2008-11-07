ACLOCAL_AMFLAGS=-I m4
AM_CXXFLAGS=-g -Wall $(DIST_INC) $(BOOST_INC)
AM_LDFLAGS=$(BOOST_LIB) -lprotobuf $(DIST_LIB)

lib_LIBRARIES=libgraphics.a

libgraphics_a_SOURCES=rpc.cpp graphics_client.cpp graphics.pb.cc
libgraphics_a_CXXFLAGS=$(DIST_INC) $(BOOST_INC)

bin_PROGRAMS=robot test_rpc_server test_rpc_client graphics_server

test_rpc_server_SOURCES=test_rpc_server.cpp
test_rpc_server_LDADD=$(lib_LIBRARIES)

test_rpc_client_SOURCES=test_rpc_client.cpp
test_rpc_client_LDADD=$(lib_LIBRARIES)

graphics_server_SOURCES=graphics.cpp
graphics_server_LDADD=$(lib_LIBRARIES)
