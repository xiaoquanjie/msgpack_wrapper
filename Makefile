ALL_DIR_SRC 	= ./ ./msgpack 
INCLUDES 	= -Imsgpack
CPPFLAGS	= -pthread -gdwarf-2

all : project

include Makefile.inc

project : $(ALL_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^
