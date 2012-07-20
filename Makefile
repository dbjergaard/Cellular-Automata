CC=g++
INCDIR=$(PWD)/include
WFLAGS=-Wall -Wextra
CFLAGS=-m64 -pg -ggdb -I$(INCDIR) -O0 $(WFLAGS) #-I$(ROOTSYS)/include 
LDFLAGS=-m64 -pg 

all: RunSim PostProcess

RunSim: RunSim.o 
	$(CC) RunSim.o -o RunSim $(LDFLAGS) 
RunSim.o:
	$(CC) $(CFLAGS) -c ./bin/RunSim.C
PostProcess: PostProcess.o 
	$(CC) PostProcess.o -o PostProcess $(LDFLAGS) 
PostProcess.o:
	$(CC) $(CFLAGS) -c ./bin/PostProcess.C
clean:
	rm -rf *o  RunSim *.xpm
