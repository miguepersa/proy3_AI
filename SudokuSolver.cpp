#include "SudokuSolver.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <filesystem>

int SudokuSolver::getVariable(int row, int col, int num) const {
    return row * 81 + col * 9 + num + 1;
}

void SudokuSolver::addCellConstraints(std::vector<std::vector<int>>& cnf) {
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            std::vector<int> clause;
            for (int num = 0; num < SIZE; ++num) {
                clause.push_back(getVariable(row, col, num));
            }
            cnf.push_back(clause);
        }
    }
    
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            for (int num1 = 0; num1 < SIZE; ++num1) {
                for (int num2 = num1 + 1; num2 < SIZE; ++num2) {
                    cnf.push_back({-getVariable(row, col, num1), -getVariable(row, col, num2)});
                }
            }
        }
    }
}

void SudokuSolver::addRowConstraints(std::vector<std::vector<int>>& cnf) {
    for (int row = 0; row < SIZE; ++row) {
        for (int num = 0; num < SIZE; ++num) {
    
            std::vector<int> clause;
            for (int col = 0; col < SIZE; ++col) {
                clause.push_back(getVariable(row, col, num));
            }
            cnf.push_back(clause);
            
    
            for (int col1 = 0; col1 < SIZE; ++col1) {
                for (int col2 = col1 + 1; col2 < SIZE; ++col2) {
                    cnf.push_back({-getVariable(row, col1, num), -getVariable(row, col2, num)});
                }
            }
        }
    }
}

void SudokuSolver::addColumnConstraints(std::vector<std::vector<int>>& cnf) {
    for (int col = 0; col < SIZE; ++col) {
        for (int num = 0; num < SIZE; ++num) {
    
            std::vector<int> clause;
            for (int row = 0; row < SIZE; ++row) {
                clause.push_back(getVariable(row, col, num));
            }
            cnf.push_back(clause);
            
    
            for (int row1 = 0; row1 < SIZE; ++row1) {
                for (int row2 = row1 + 1; row2 < SIZE; ++row2) {
                    cnf.push_back({-getVariable(row1, col, num), -getVariable(row2, col, num)});
                }
            }
        }
    }
}

void SudokuSolver::addBoxConstraints(std::vector<std::vector<int>>& cnf) {
    for (int boxRow = 0; boxRow < BOX_SIZE; ++boxRow) {
        for (int boxCol = 0; boxCol < BOX_SIZE; ++boxCol) {
            for (int num = 0; num < SIZE; ++num) {
        
                std::vector<int> clause;
                for (int row = 0; row < BOX_SIZE; ++row) {
                    for (int col = 0; col < BOX_SIZE; ++col) {
                        clause.push_back(getVariable(boxRow * BOX_SIZE + row, boxCol * BOX_SIZE + col, num));
                    }
                }
                cnf.push_back(clause);
                
        
                for (int pos1 = 0; pos1 < SIZE; ++pos1) {
                    for (int pos2 = pos1 + 1; pos2 < SIZE; ++pos2) {
                        int row1 = boxRow * BOX_SIZE + pos1 / BOX_SIZE;
                        int col1 = boxCol * BOX_SIZE + pos1 % BOX_SIZE;
                        int row2 = boxRow * BOX_SIZE + pos2 / BOX_SIZE;
                        int col2 = boxCol * BOX_SIZE + pos2 % BOX_SIZE;
                        cnf.push_back({-getVariable(row1, col1, num), -getVariable(row2, col2, num)});
                    }
                }
            }
        }
    }
}

void SudokuSolver::addPuzzleConstraints(const std::string& puzzle, std::vector<std::vector<int>>& cnf) {
    for (int i = 0; i < puzzle.size(); ++i) {
        char c = puzzle[i];
        if (c != '.') {
            int num = c - '1';
            int row = i / SIZE;
            int col = i % SIZE;
            int var = getVariable(row, col, num);
            cnf.push_back({var});
        }
    }
}

std::string SudokuSolver::decodeSolution(const std::unordered_map<int, bool>& model) {
    std::string solution(SIZE * SIZE, '.');
    int filled = 0;

    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            for (int num = 0; num < SIZE; ++num) {
                int var = getVariable(row, col, num);
                if (model.count(var) && model.at(var)) {
                    solution[row * SIZE + col] = '1' + num;
                    filled++;
                    break;
                }
            }
        }
    }

    return solution;
}

std::string SudokuSolver::solve(const std::string& puzzle) {
    if (puzzle.size() != SIZE * SIZE) {
        return "Invalid puzzle size";
    }
    
    std::vector<std::vector<int>> cnf;
    
    addCellConstraints(cnf);
    addRowConstraints(cnf);
    addColumnConstraints(cnf);
    addBoxConstraints(cnf);
    addPuzzleConstraints(puzzle, cnf);
    
    std::string tempFilename = "sudoku_temp.cnf";
    std::ofstream out(tempFilename);
    
    int maxVar = SIZE * SIZE * SIZE;
    out << "p cnf " << maxVar << " " << cnf.size() << "\n";
    
    for (const auto& clause : cnf) {
        for (int lit : clause) {
            out << lit << " ";
        }
        out << "0\n";
    }
    out.close();
    
    bool sat = solver.solve(tempFilename);
    std::remove(tempFilename.c_str());
    
    if (!sat) {
        return "UNSATISFIABLE";
    }
    
    const auto& model = solver.getModel();
    std::string solution = decodeSolution(model);
    
    return solution;
}