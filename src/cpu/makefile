CC = gcc
LD = $(CC)

APP = ap26_cpu_linux64

SRC = ../common/AP26.cpp ../common/CONST.H ../common/PrimeQ_x86.h
OBJ = AP26.o cpuavx2.o cpuavx.o cpusse41.o cpusse2.o

BOINC_DIR = /home/bryan/boinc
BOINC_INC = -I$(BOINC_DIR)/api -I$(BOINC_DIR)/lib -I$(BOINC_DIR)
BOINC_LIB = -L$(BOINC_DIR)/lib -L$(BOINC_DIR)/api -L$(BOINC_DIR) -L. -lboinc_api -lboinc -lstdc++ -lpthread -lm

DFLAGS  = -DAP26_BOINC -DAP26_CPU
CFLAGS  = -I . -O3 -m64
LDFLAGS = $(CFLAGS) -static-libgcc

all : $(APP)

$(APP) : $(OBJ) 
	$(LD) $(LDFLAGS) $^ $(BOINC_LIB) -o $@

AP26.o : $(SRC)
	$(CC) $(DFLAGS) $(CFLAGS) $(BOINC_INC) -c ../common/AP26.cpp

cpuavx2.o : cpuavx2.cpp
	$(CC) $(DFLAGS) $(CFLAGS) -mavx2 -c cpuavx2.cpp

cpuavx.o : cpuavx.cpp
	$(CC) $(DFLAGS) $(CFLAGS) -mavx -c cpuavx.cpp

cpusse41.o : cpusse41.cpp
	$(CC) $(DFLAGS) $(CFLAGS) -msse4.1 -c cpusse41.cpp

cpusse2.o : cpusse2.cpp
	$(CC) $(DFLAGS) $(CFLAGS) -msse2 -c cpusse2.cpp

clean :
	rm -f *.o $(APP)

