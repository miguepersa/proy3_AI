#include "DPLLSolver.h"
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <iostream>

void DPLLSolver::parseDIMACS(const std::string& filename, CNF& clauses, std::vector<int>& symbols) {
    std::ifstream in(filename);
    std::string line;
    std::unordered_set<int> symbol_set;

    while (std::getline(in, line)) {
        if (line.empty() || line[0] == 'c') continue;
        if (line[0] == 'p') continue;

        std::istringstream iss(line);
        int lit;
        Clause clause;
        while (iss >> lit) {
            if (lit == 0) break;
            clause.push_back(lit);
            symbol_set.insert(std::abs(lit));
        }
        clauses.push_back(clause);
    }

    symbols.assign(symbol_set.begin(), symbol_set.end());
}

bool DPLLSolver::solve(const std::string& filename) {
    CNF clauses;
    std::vector<int> symbols;
    parseDIMACS(filename, clauses, symbols);
    std::sort(symbols.begin(), symbols.end());
    Model model;

    auto start = std::chrono::high_resolution_clock::now();
    bool result = dpll(clauses, symbols, model);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << (result ? "SATISFIABLE" : "UNSATISFIABLE") << "\n";
    std::chrono::duration<double> duration = end - start;
    std::cout << duration.count() << "\n";

    return result;
}

CNF DPLLSolver::simplifyClauses(const CNF& clauses, const Model& model) {
    CNF simplified;
    for (const auto& clause : clauses) {
        Clause new_clause;
        bool clause_satisfied = false;
        for (int lit : clause) {
            int var = std::abs(lit);
            auto it = model.find(var);
            if (it != model.end()) {
                if ((lit > 0 && it->second) || (lit < 0 && !it->second)) {
                    clause_satisfied = true;
                    break;
                } else {
                    continue; // Literal es falso
                }
            }
            new_clause.push_back(lit); // Literal sin asignar
        }
        if (!clause_satisfied) {
            simplified.push_back(new_clause);
        }
    }
    return simplified;
}

bool DPLLSolver::dpll(CNF& clauses, std::vector<int>& symbols, Model& model) {
    CNF simplified = simplifyClauses(clauses, model);

    if (simplified.empty()) return true;

    for (const auto& clause : simplified) {
        if (clause.empty()) return false;
    }

    std::vector<int> remaining_symbols;
    std::copy_if(symbols.begin(), symbols.end(), std::back_inserter(remaining_symbols),
                 [&model](int s) { return model.find(s) == model.end(); });

    auto [pure_sym, pure_val] = findPureSymbol(remaining_symbols, simplified, model);
    if (pure_sym != 0) {
        Model new_model = model;
        new_model[pure_sym] = pure_val;
        return dpll(clauses, symbols, new_model);
    }

    auto [unit_sym, unit_val] = findUnitClause(simplified, model);
    if (unit_sym != 0) {
        Model new_model = model;
        new_model[unit_sym] = unit_val;
        return dpll(clauses, symbols, new_model);
    }

    int P = remaining_symbols.front();

    Model model_true = model;
    model_true[P] = true;
    if (dpll(clauses, symbols, model_true)) return true;

    Model model_false = model;
    model_false[P] = false;
    return dpll(clauses, symbols, model_false);
}


bool DPLLSolver::isClauseTrue(const Clause& clause, const Model& model) {
    for (int lit : clause) {
        int var = std::abs(lit);
        auto it = model.find(var);
        if (it != model.end() && ((lit > 0 && it->second) || (lit < 0 && !it->second)))
            return true;
    }
    return false;
}

bool DPLLSolver::isClauseFalse(const Clause& clause, const Model& model) {
    for (int lit : clause) {
        int var = std::abs(lit);
        auto it = model.find(var);
        if (it == model.end() || (lit > 0 && it->second) || (lit < 0 && !it->second))
            return false;
    }
    return true;
}

std::pair<int, bool> DPLLSolver::findPureSymbol(const std::vector<int>& symbols, const CNF& clauses, const Model& model) {
    std::unordered_map<int, int> polarity;
    for (const auto& clause : clauses) {
        for (int lit : clause) {
            int var = std::abs(lit);
            if (model.count(var)) continue;
            if (lit > 0) polarity[var] |= 1;
            else polarity[var] |= 2;
        }
    }
    for (int s : symbols) {
        if (polarity[s] == 1) return {s, true};
        if (polarity[s] == 2) return {s, false};
    }
    return {0, false};
}

std::pair<int, bool> DPLLSolver::findUnitClause(const CNF& clauses, const Model& model) {
    for (const auto& clause : clauses) {
        int unassigned = 0;
        int literal = 0;
        bool satisfied = false;
        for (int lit : clause) {
            int var = std::abs(lit);
            auto it = model.find(var);
            if (it == model.end()) {
                ++unassigned;
                literal = lit;
            } else if ((lit > 0 && it->second) || (lit < 0 && !it->second)) {
                satisfied = true;
                break;
            }
        }
        if (!satisfied && unassigned == 1) {
            int var = std::abs(literal);
            return {var, literal > 0};
        }
    }
    return {0, false};
}
