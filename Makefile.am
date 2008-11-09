ACLOCAL_AMFLAGS=-I m4
AUTOMAKE_OPTIONS=foreign

AM_CXXFLAGS=@CXXFLAGS@
# AM_LDFLAGS=$(BOOST_LIB) -lprotobuf $(DIST_LIB)

lib_LIBRARIES=libgraphics.a

libgraphics_a_SOURCES=rpc.cpp graphics_client.cpp graphics.pb.cc

bin_PROGRAMS=test_rpc_server test_graphics_client test_rpc_client graphics_server

test_rpc_server_SOURCES=test_rpc_server.cpp

test_graphics_client_SOURCES=test_graphics_client.cpp

test_rpc_client_SOURCES=test_rpc_client.cpp

graphics_server_SOURCES=graphics.cpp
