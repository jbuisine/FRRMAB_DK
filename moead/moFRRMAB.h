#ifndef __FRRMAB__h
#define __FRRMAB__h
//#include "mpicxx.h"

#include <map>
#include <vector>
#include <fstream>
#include "subProblems.h"
#include "init.h"
#include "repair.h"
#include "operators/mutation.h"
#include "moAlgo.h"
#include "paretoFront.h"
#include "hyperVolume.h"
//#include "checkSol.h"

/**
   FRRMAB algorithm proposed by "Adaptive Operator Selection With Bandits for a Multiobjective Evolutionary Algorithm Based on Decomposition" article
 **/
class FRRMAB : public MultiObjectiveAlgo {
public:
    FRRMAB(moEval &_eval, SubProblems &_subproblems, bool _pbType, Init &_init, std::vector<Operator*> &_operators, Repair &_repair,
           unsigned _mu, double _C, double _D, unsigned _maxEval)
            : evaluation(_eval), subProblems(_subproblems), pbType(_pbType), initialization(_init), operators(_operators), repair(_repair), mu(_mu),
              C(_C), D(_D), maxEval(_maxEval)  {

        unsigned nbOperators = operators.size();

        // Initialization
        // nop : number of op present into each slidingWindow
        nop.resize(nbOperators);
        // non selected op
        unusedOp.resize(nbOperators);
        // resize pop to mu
        pop.resize(mu);
        // check if necessary to use an unused op
        checkUnusedOp = true;

        for (int i = 0; i < mu; i++) {
            pop.at(i).resize(evaluation.size());
        }

        for (int j = 0; j < nbOperators; j++) {
            unusedOp.at(j) = true;

            // init each pair(OpID, FFR value) of sub problem
            FRRs.insert(std::make_pair(j, 0.));
        }
    }

    /**
     * FRRMAB with sliding window for each N sub problem implementation
     *
     * @param fileout
     */
    virtual void run(const char *fileout) {
        //simpleHashMap sHM;
        unsigned nbEval = 0;
        time_t begintime = time(NULL);

        // convert pop into pf
        ParetoFront paretoFront;

        // output file header
        FILE *file = fopen(fileout, "w");

        // initialization of the population
        pop.resize(mu);
        for (unsigned i = 0; i < mu; i++) {

            initialization(pop[i]);

            // repair sol at init
            // pop[i] = repair(pop[i], i, pbType);

            evaluation(pop[i]);
            pop[i].fitness(subProblems.scalarfunc(i, pop[i]));

            //sHM.insertSol(pop[i]);
            nbEval++;
            pop[i].ID(nbEval);
            pop[i].dir(i);
            pop[i].from(-1);
            pop[i].best(1);
            pop[i].op(-1);
            pop[i].save(file);
            // add this solution into pf pop
            pfPop.push_back(pop[i]);

            // update min ref point to compute HV
            updateMinRefPoint(pop[i]);
        }

        moSolution mutant;

        // stopping criterium is the number of seconds
        unsigned i = 0;
        while (nbEval < maxEval) {
            // mutation of the direction i

            // get best next op for sub problem

            int selectedOpIndex = getBestOp();
            Operator& op = *operators.at(selectedOpIndex);

            mutant = pop[i];
            mutant.best(0);
            mutant.op(selectedOpIndex);

            // TODO use of repair function (local search)
            //while (!sHM.isNewSol(mutant)) {

                // get mutant solution from operator choose
                if(op.getKind() == OpKind::CROSSOVER){

                    mutant = op(mutant, pop[rand() % pop.size()]);

                }else if(op.getKind() == OpKind::MUTATION){

                    mutant = op(mutant);
                }

                //mutant = repair(mutant, i, pbType);
            //}

            //sHM.insertSol(mutant);
            evaluation(mutant);

            // update min ref point to compute HV
            updateMinRefPoint(mutant);

            nbEval++;
            mutant.ID(nbEval);

            double fitness;
            std::vector<unsigned> neighbors = subProblems.neighborProblems(i);

            double FIRop = 0.;

            for (unsigned n : neighbors) {

                // getting fir value for this neighbor
                double fitness = subProblems.scalarfunc(n, mutant);
                double fir = computeFIR(pop[n], fitness, n);

                // if fir is better than 0 (we have amelioration)
                if (fir > 0) {
                    // replace solution
                    pop[n] = mutant;

                    // set new fitness value
                    pop[n].fitness(fitness);

                    mutant.best(1);

                    // set new FIR op used
                    FIRop += fir;
                }
            }

            if(FIRop > 0){
                pfPop.push_back(pop[i]);
                pfPop = paretoFront(pfPop, pbType);
            }

            // set new FIR value into sliding window
            updateSlidingWindow(selectedOpIndex, FIRop);

            // update credit assignment values
            updateCreditAssignmentSubProblem();

            mutant.op(selectedOpIndex);
            mutant.save(file);

            // next direction
            i++;
            if (i >= mu) {
                i = 0;
                HyperVolume hv;

                //std::cout << "POP : " << hv(pop) << std::endl;
                std::cout << "PF : " << hv(pfPop, minRefPoint) << std::endl;
                std::cout << ((double)nbEval / (double)maxEval) * 100. << "%" << std::endl;
            };
        }

        // set duration time
        duration = time(NULL) - begintime;
    }

protected:
    moEval &evaluation;
    SubProblems &subProblems;
    Init &initialization;
    std::vector<Operator*> &operators;
    Repair &repair;
    unsigned mu;

    /**
    * Adaptive Operator Selection variables
    */
    // FRR values
    std::map<int, double> FRRs;
    // nop : number of op present of slidingWindow
    std::vector<int> nop;
    // non selected op sub problem
    std::vector<bool> unusedOp;
    // value which will specify if necessary to check unused op
    bool checkUnusedOp;
    // slidingWindow
    std::vector<std::pair<int, double> > slidingWindow;
    // C : UCB scaling factor which control the trade-off EvE
    double C;
    // D decaying factor D in [0,1]
    double D;
    // max evaluation of algo
    unsigned maxEval;
    // king of problem (minimize true or maximize false)
    bool pbType;

private:

    /**
     * Update sliding window content for the _subProblem
     *
     * @param _selectedOpIndex
     * @param _FIROp
     */
    void updateSlidingWindow(int _selectedOpIndex, double _FIROp) {

        // add new element
        slidingWindow.push_back(std::make_pair(_selectedOpIndex, _FIROp));

        // remove old element if necessary
        if (slidingWindow.size() > subProblems.getW()) {
            slidingWindow.erase(slidingWindow.begin());
        }
    }

    /**
     * FIR compute value
     *
     * @param _solution
     * @param _mutantFitness : fitness value of mutant
     * @return
     */
    double computeFIR(moSolution &_solution, double _mutantFitness, int _n) {
        double solFitness = subProblems.scalarfunc(_n, _solution);

        // if minimize
        if(pbType)
            return (solFitness - _mutantFitness) / solFitness;
        else
            return (_mutantFitness - solFitness) / solFitness;
    }

    /**
     * Credit assignment updates
     */
    void updateCreditAssignmentSubProblem() {

        // Init vector rewards and op values
        std::vector<double> rewards(operators.size());

        for (int i = 0; i < rewards.size(); i++) {
            rewards.at(i) = 0.;
            nop.at(i) = 0;
        };

        // For each elements into slidingWindow of subProblem
        for (int i = 0; i < slidingWindow.size(); i++) {

            unsigned op = slidingWindow.at(i).first;
            double FIR = slidingWindow.at(i).second;

            // update reward
            rewards.at(op) += FIR;
            nop.at(op)++;
        }

        // getting rank of each reward of operator
        std::vector<unsigned > opRanks(operators.size());

        // set default indexes
        std::size_t n(0);
        std::generate(std::begin(opRanks), std::end(opRanks), [&]{ return n++; });

        // getting rank indexes sorted
        std::sort(opRanks.begin(), opRanks.end(), [&](int i1, int i2) { return rewards[i1] > rewards[i2]; } );

        // Compute decay values of each op and decaySum
        std::vector<double> decays(operators.size());
        double decaySum = 0.;

        for (unsigned op = 0; op < operators.size(); op++) {

            unsigned rank = std::find(opRanks.begin(), opRanks.end(), op) - opRanks.begin();
            decays.at(op) = pow(D, (rank + 1)) * rewards.at(op);
            decaySum += decays.at(op);
        }

        // Compute new FRRs value
        for (unsigned op = 0; op < operators.size(); op++) {
            if(decaySum != 0)
                FRRs.at(op) = decays.at(op) / decaySum;
            else
                FRRs.at(op) = 0.;
        }
    }

    /**
     * Return best next op for subProblem FFRMABN version
     *
     * take consideration of neighborhood information
     *
     * @param _subProblem
     * @return
     */
    unsigned getBestOp() {

        unsigned selectedOp;

        // check if all operator are used at least once
        if (checkUnusedOp) {

            std::vector<int> unusedOpIndexes;

            for (unsigned i = 0; i < unusedOp.size(); i++) {
                if (unusedOp.at(i)) {
                    unusedOpIndexes.push_back(i);
                }
            }

            // all op will be used at least once after this iteration
            if (unusedOpIndexes.size() <= 1)
                checkUnusedOp = false;

            int randIndex = rand() % unusedOpIndexes.size();

            // getting next op choose randomly
            selectedOp = unusedOpIndexes.at(randIndex);

            // set op as viewed
            unusedOp.at(selectedOp) = false;

        } else {

            // Use UCB to best next op
            double maxValue = -1.;

            // sum of number operator (sliding window size)
            int nopSum = 0;

            for (unsigned i = 0; i < nop.size(); ++i) {
                nopSum += nop.at(i);
            }

            // search best op at time t for subProblem
            for (unsigned i = 0; i < FRRs.size(); i++) {

                double explorationValue = sqrt((2 * log(nopSum)) / nop.at(i));
                double currentValue = FRRs.at(i) + (C * explorationValue);

                if (currentValue > maxValue) {

                    // random choice to change op value if equal
                    if(currentValue == maxValue){
                        double p = ((double) rand() / (RAND_MAX));

                        if(p < 0.5){
                            continue;
                        }
                    }

                    maxValue = currentValue;
                    selectedOp = i;
                }
            }
        }
        return selectedOp;
    }
};// end FFRMAB
#endif
