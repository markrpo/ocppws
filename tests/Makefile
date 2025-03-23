SOURCEDIR=../src

TESTS=$(wildcard *.cpp)
OBJS=$(TESTS:.cpp=.o)
EXE=tests
CXX=g++
#SOURCEOBJS=$(wildcard $(SOURCEDIR)/*.o)
#SOURCEOBJS := $(filter-out $(SOURCEDIR)/main.o, $(SOURCEOBJS))
# Enter manually the .o files from the source directory
SOURCEOBJS=$(SOURCEDIR)/ocpp.o $(SOURCEDIR)/responses.o

# Wall: all warnings on -std=c++17: c++17 standard -I../src: include path
CXXFLAGS=-std=c++17 -Wall -I$(SOURCEDIR)
# LDFLAGS: link flags
LDFLAGS=-lgtest -lgtest_main -lgmock -lgmock_main -pthread

all: $(EXE)

$(EXE): $(OBJS) $(SOURCEOBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXE)
	$(MAKE) -C $(SOURCEDIR) clean

%.o: %.cpp
