EXE=gardner_etal_2011.exe
CPP=individual.cpp simulation.cpp gardner2011.cpp patch.cpp
HPP=individual.hpp simulation.hpp parameters.hpp patch.hpp
CXX=g++
CXXFLAGS=-Wall -O3 -ggdb -std=c++17

$(EXE) : $(CPP) $(HPP)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(CPP)

clean :
	rm -rf $(EXE)
