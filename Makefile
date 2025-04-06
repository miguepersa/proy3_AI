CC=g++
CFLAGS= -Wall -Wextra -std=c++11
SAT_OBJ= DPLLSolver.o satSolverMain.o
SUDOKU_OBJ= DPLLSolver.o SudokuSolver.o sudokuSolverMain.o

all: sudokuSolver dpllSolver

sudokuSolver: $(SUDOKU_OBJ)
	$(CXX) $(CXXFLAGS) $(SUDOKU_OBJ) -o sudokuSolver

dpllSolver: $(SAT_OBJ)
	$(CXX) $(CXXFLAGS) $(SAT_OBJ) -o dpllSolver

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o sudokuSolver dpllSolver
