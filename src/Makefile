SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
dep=$(OBJ:.o=.d)
EXE=ocppws
CXX=g++
# Wall: all warnings on -std=c++17: C++17 standard -I.: include path
CXXFLAGS=-std=c++17 -Wall -I.
LDFLAGS=-lwebsockets

all: $(EXE)

# $@: target name $^: all dependencies $<: first dependency
$(EXE): $(OBJ)
	@echo "Linking $^ to $@"
	$(CXX) $^ -o $@ $(LDFLAGS)

-include $(dep)

# MMD: generate dependencies (header files .d)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	rm -f $(OBJ) $(EXE) $(dep)

# .PHONY: target is not a file this is used to avoid conflicts with files
.PHONY: all clean
