SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
dep=$(OBJ:.o=.d)
JSONS=$(wildcard ./v16/schemas/*.json)
JSON_HEADERS=$(JSONS:.json=.hpp)
JSON_HEADER=jsons_headers.hpp
VALIJSON_HEADER=valijson_nlohmann.hpp
EXE=ocppws
CXX=g++
# Wall: all warnings on -std=c++17: C++17 standard -I.: include path
CXXFLAGS=-std=c++17 -Wall -I.
LDFLAGS=-lwebsockets
INCLUDES=-I.

all: $(EXE)

# $@: target name $^: all dependencies $<: first dependency
$(EXE): $(OBJ)
	@echo "Linking $^ to $@"
	$(CXX) $^ -o $@ $(LDFLAGS)

-include $(dep)

# MMD: generate dependencies (header files .d)
%.o: %.cpp $(JSON_HEADER)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@ $(INCLUDES)


# Generate JSON header files from JSON files
%.hpp: %.json
	@echo "Generating $@ from $<"
	@echo "#ifndef $(shell echo $@ | tr / _ | tr . _ | tr a-z A-Z)_H" > $@
	@echo "#define $(shell echo $@ | tr / _ | tr . _ | tr a-z A-Z)_H" >> $@
	xxd -i $< >> $@
	@echo "#endif // $(shell echo $@ | tr / _ | tr . _ | tr a-z A-Z)_H" >> $@

$(JSON_HEADER): $(JSON_HEADERS)
	@echo "Generating JSON header"
	@echo "#ifndef JSONS_HEADERS_HPP" > $@
	@echo "#define JSONS_HEADERS_HPP" >> $@
	@for header in $(JSON_HEADERS); do \
		echo "#include \"$$header\"" >> $@; \
	done  
	@echo "#endif // JSONS_HEADERS_HPP" >> $@

header: $(JSON_HEADER) $(VALIJSON_HEADER)

$(VALIJSON_HEADER):
	@git clone git@github.com:tristanpenman/valijson.git
	@cd valijson && git checkout v1.0.5
	@cd valijson && ./bundle.sh nlohmann_json > valijson_nlohmann_bundled.hpp
	@mv valijson/valijson_nlohmann_bundled.hpp $(VALIJSON_HEADER)

clean:
	rm -rf $(OBJ) $(EXE) $(dep) $(JSON_HEADERS) $(JSON_HEADER) $(VALIJSON_HEADER) valijson

# .PHONY: target is not a file this is used to avoid conflicts with files
.PHONY: all clean
