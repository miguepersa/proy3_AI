#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include "DPLLSolver.h"
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

class SudokuSolver {
public:
    SudokuSolver() : solver() {}
    std::string solve(const std::string& puzzle);
    
private:
    DPLLSolver solver;
    
    void addCellConstraints(std::vector<std::vector<int>>& cnf);
    void addRowConstraints(std::vector<std::vector<int>>& cnf);
    void addColumnConstraints(std::vector<std::vector<int>>& cnf);
    void addBoxConstraints(std::vector<std::vector<int>>& cnf);
    void addPuzzleConstraints(const std::string& puzzle, std::vector<std::vector<int>>& cnf);
    
    std::string decodeSolution(const std::unordered_map<int, bool>& model);
    
    int getVariable(int row, int col, int num) const;
    
    static const int SIZE = 9;
    static const int BOX_SIZE = 3;
};

#endif