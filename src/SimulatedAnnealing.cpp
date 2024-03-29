#include "SimulatedAnnealing.hpp"

using namespace std;


SimulatedAnnealing::SimulatedAnnealing(
    Temperature temperature,
    Solution* initial_solution,
    Journal* journal,
    int L
):
    temperature(temperature),
    initial_solution(initial_solution),
    journal(journal),
    L(L),
    accepted_global(0)
{}

SimulatedAnnealing::~SimulatedAnnealing() {
    delete journal;
    delete initial_solution;
}

Solution* SimulatedAnnealing::TresholdAccepting() {
    double p = 0;
    Solution* best_solution = new Solution(initial_solution);
    Solution* solution = initial_solution;
    while (temperature.BiggerThanZero()) {
        double q = DBL_MAX;
        while (p <= q) {
            q = p;
            p = ComputeBatch(solution, &best_solution);
        }
        temperature.Decrease();
    }
    return best_solution;
}

Solution* SimulatedAnnealing::TresholdAcceptingSweep() {
    Solution* best = TresholdAccepting();
    while (best->Sweep()) {
        accepted_global++;
        journal->AppendBestSolution(accepted_global, best->GetCost());
    }
    return best;
}

double SimulatedAnnealing::ComputeBatch(
    Solution* solution, Solution** best_solution
) {
    int iteration_batch = 0;
    int c = 0;
    double r = 0;
    double temp = temperature.GetTemperature();
    while (c < L && iteration_batch++ < STOP) {
        double cost = solution->GetCost();
        if (iteration_batch % PERIOD == 0)
            solution->MorphIntoNeighbour(true);
        else
            solution->MorphIntoNeighbour(false);
        
        if (solution->GetCost() < (*best_solution)->GetCost()) {
            delete *best_solution;
            *best_solution = new Solution(solution);
            journal->AppendBestSolution(
                accepted_global, (*best_solution)->GetCost()
            );
        }
            
        if (solution->GetCost() <= cost + temp) {  // Solution gets accepted
            c++;
            accepted_global++;
            r += solution->GetCost();
            journal->AppendAcceptedSolution(
                accepted_global, solution->GetCost()
            );
        } else
            solution->MorphBack();
    }
    return r / c;
}

void SimulatedAnnealing::WriteToJournalFiles() {
    journal->WriteToJournalFiles();
}
