#include <iostream>
#include "DPLLSolver.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./sat_solver <input_file.cnf>\n";
        return 1;
    }

    DPLLSolver solver;
    solver.solve(argv[1]);
    return 0;
}
