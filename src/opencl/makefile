# makefile for openclapp BOINC example appication using AMD GPU on Linux.

BOINC_DIR = /home/bryan/boinc
BOINC_API_DIR = $(BOINC_DIR)/api
BOINC_LIB_DIR = $(BOINC_DIR)/lib 
AMDAPPSDKROOT = /opt/AMDAPPSDK-3.0

CXXFLAGS = -g -O3 -m64 \
    -I$(BOINC_DIR) \
    -I$(BOINC_LIB_DIR) \
    -I$(BOINC_API_DIR) \
    -I$(AMDAPPSDKROOT)/include/CL \
    -L. \
    -L$(BOINC_LIB_DIR) \
    -L$(BOINC_API_DIR) \
    -L$(AMDAPPSDKROOT)/lib/x86_64/sdk \
    -L/lib/i386-linux-gnu
## The above 2 lines work around an Ubuntu LD_LIBRARY_PATH bug described in: 
## <https://help.ubuntu.com/community/EnvironmentVariables#File-location_related_variables>

PROGS = openclapp \

all: $(PROGS)

libstdc++.a:
	ln -s `g++ -print-file-name=libstdc++.a`

clean:
	/bin/rm -f $(PROGS) *.o libstdc++.a

distclean:
	/bin/rm -f $(PROGS) *.o libstdc++.a

install: openclapp

openclapp: boinc_opencl.o libstdc++.a
	$(CXX) $(CXXFLAGS) -o ap26opencl AP26.c simpleCL.c boinc_opencl.o \
        libstdc++.a -lOpenCL -lboinc_api -lboinc -lpthread -DBOINC

boinc_opencl.o: $(BOINC_API_DIR)/boinc_opencl.cpp $(BOINC_API_DIR)/boinc_opencl.h
	$(CXX) $(CXXFLAGS) -c $(BOINC_API_DIR)/boinc_opencl.cpp

