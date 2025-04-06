#include "SudokuSolver.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <sudoku_instance>\n";
        return 1;
    }
    
    std::string puzzle = argv[1];
    SudokuSolver solver;
    std::string solution = solver.solve(puzzle);
    
    std::cout << solution << std::endl;
    
    return 0;
}