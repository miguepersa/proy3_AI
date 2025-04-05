#ifndef DPLL_SOLVER_H
#define DPLL_SOLVER_H

#include <vector>
#include <string>
#include <unordered_map>

class DPLLSolver {
public:
    bool solve(const std::string& filename);
private:
    using Clause = std::vector<int>;
    using CNF = std::vector<Clause>;
    using Model = std::unordered_map<int, bool>;

    bool dpll(CNF clauses, std::vector<int> symbols, Model model);
    bool isClauseTrue(const Clause& clause, const Model& model);
    bool isClauseFalse(const Clause& clause, const Model& model);
    std::pair<int, bool> findPureSymbol(const std::vector<int>& symbols, const CNF& clauses, const Model& model);
    std::pair<int, bool> findUnitClause(const CNF& clauses, const Model& model);
    void parseDIMACS(const std::string& filename, CNF& clauses, std::vector<int>& symbols);
    CNF simplifyClauses(const CNF&, const Model&);
};

#endif
