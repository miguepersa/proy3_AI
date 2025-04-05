CC=g++
CFLAGS= -Wall -Wextra -std=c++11
OBJ= DPLLSolver.o main.o

dpllSolver: $(OBJ)
	$(CXX) $(CXXFLAGS) -o dpllSolver $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o dpllSolver
