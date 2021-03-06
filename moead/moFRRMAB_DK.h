#ifndef __FRRMAB_DK__h
#define __FRRMAB_DK__h


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
   FRRMAB_DK which uses sliding windows for each directions and share Reward to compute FRR information of current direction
   - alpha : [0, 1], value which declare affinity of a direction and its neighbors
 **/

class FRRMAB_DK : public MultiObjectiveAlgo {
public:
    FRRMAB_DK(moEval &_eval, SubProblems &_subproblems, bool _pbType, Init &_init, std::vector<Operator*> &_operators, Repair &_repair,
           unsigned _mu, double _C, double _D, double _affinity, unsigned _maxEval)
            : evaluation(_eval), subProblems(_subproblems), pbType(_pbType), initialization(_init), operators(_operators), repair(_repair), mu(_mu),
              C(_C), D(_D), affinity(_affinity), maxEval(_maxEval)  {

        // Initialization
        // FRR values for each sub problem operators
        FRRs.resize(mu);
        // nop : number of op present into each slidingWindows (sub problem)
        nop.resize(mu);
        // non selected op sub problem
        unusedOp.resize(mu);
        // value which will specify if necessary to check unused op
        checkUnusedOp.resize(mu);
        // resize pop to mu
        pop.resize(mu);

        unsigned nbOperators = operators.size();

        for (int i = 0; i < mu; i++) {

            pop.at(i).resize(evaluation.size());

            checkUnusedOp.at(i) = true;

            // init each sub vectors size
            unusedOp.at(i).resize(nbOperators);
            nop.at(i).resize(nbOperators);

            for (int j = 0; j < nbOperators; j++) {
                unusedOp.at(i).at(j) = true;

                // init each pair(OpID, FFR value) of sub problem
                FRRs.at(i).insert(std::make_pair(j, 0.));
            }
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

            //pop[i] = repair(pop[i], i, pbType);

            evaluation(pop[i]);
            pop[i].fitness(subProblems.scalarfunc(i, pop[i]));
            //repair(pop[i], i, true);
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
            int selectedOpIndex = getBestOp(i);
            Operator& op = *operators.at(selectedOpIndex);

            mutant = pop[i];
            mutant.best(0);

            mutant.op(selectedOpIndex);

            // TODO use of repair function (local search)
            //while (!sHM.isNewSol(mutant)) {

                // get mutant solution from operator choose
                if(op.getKind() == OpKind::CROSSOVER){

                    unsigned index = rand() % pop.size();
                    mutant = op(mutant, pop[index]);

                }else if(op.getKind() == OpKind::MUTATION){

                    mutant = op(mutant);
                }

            evaluation(mutant);

            updateMinRefPoint(mutant);
            //mutant = repair(mutant, i, pbType);

            //}

            //sHM.insertSol(mutant);
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
                pfPop.push_back(mutant);
                pfPop = paretoFront(pfPop, pbType);
            }

            // set new FIR value into sliding window of sub problem i
            updateSlidingWindow(i, selectedOpIndex, FIRop);

            // update credit assignment value of sub problem i
            updateCreditAssignmentSubProblem(i);

            mutant.op(selectedOpIndex);
            mutant.save(file);

            // next direction
            i++;
            if (i >= mu) {
                i = 0;
                HyperVolume hv;
                //std::cout << "POP : " << hv(pop, minRefPoint) << std::endl;
                std::cout << "PF : " << hv(pfPop, minRefPoint) << std::endl;
                std::cout << ((double)nbEval / (double)maxEval) * 100. << "%" << std::endl;
            }
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
    // FRR values for each sub problem operators
    std::vector<std::map<int, double> > FRRs;
    // nop : number of op present into each slidingWindows
    std::vector<std::vector<int> > nop;
    // non selected op sub problem
    std::vector<std::vector<bool> > unusedOp;
    // value which will specify if necessary to check unused op
    std::vector<bool> checkUnusedOp;
    // C : UCB scaling factor which control the trade-off EvE
    double C;
    // D decaying factor D in [0,1]
    double D;
    // affinity criteria to compute Reward values
    double affinity;
    // max evaluation of algo
    unsigned maxEval;
    // king of problem (minimize true or maximize false)
    bool pbType;

private:

    /**
     * Update sliding window content for the _subProblem
     *
     * @param _subProblem
     * @param _selectedOpIndex
     * @param _FIROp
     */
    void updateSlidingWindow(int _subProblem, int _selectedOpIndex, double _FIROp) {

        // add new element
        subProblems.slidingWindows.at(_subProblem).push_back(std::make_pair(_selectedOpIndex, _FIROp));

        // remove old element if necessary
        if (subProblems.slidingWindows.at(_subProblem).size() > subProblems.getW()) {
            subProblems.slidingWindows.at(_subProblem).erase(subProblems.slidingWindows.at(_subProblem).begin());
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
     * Credit assignment updates of a subProblem
     *
     * @param _subProblem
     */
    void updateCreditAssignmentSubProblem(unsigned _subProblem) {

        unsigned nbOperators = operators.size();

        // Init vector rewards and op values
        std::vector<double> rewards(nbOperators);

        for (int i = 0; i < nbOperators; i++) {
            rewards.at(i) = 0.;
            nop.at(_subProblem).at(i) = 0;
        };

        // adding rewards of neighbors with affinity criteria (include itself)
        std::vector<unsigned > neighbors = subProblems.neighborProblems(_subProblem);

        for(unsigned n : neighbors){

            std::vector<double> rewardsNeighbor(nbOperators);
            std::vector<unsigned> nopNeighbors(nbOperators);

            for (unsigned i = 0; i < nbOperators; i++) {
                rewardsNeighbor.at(i) = 0.;
                nopNeighbors.at(i) = 0;
            }

            for (unsigned i = 0; i < subProblems.slidingWindows.at(n).size(); i++) {

                unsigned op = subProblems.slidingWindows.at(n).at(i).first;
                double FIR = subProblems.slidingWindows.at(n).at(i).second;

                // update reward
                rewardsNeighbor.at(op) += FIR;
                nopNeighbors.at(op)++;
            }

            // add new values based on affinity criteria
            unsigned neighborDist = abs((int)_subProblem - (int)n);

            for(unsigned i = 0; i < nbOperators; i++){

                // only take in consideration current sub problem for number of op
                //if(n == _subProblem) {
                    nop.at(_subProblem).at(i) += nopNeighbors.at(i);
                //}
                    //rewards.at(i) += rewardsNeighbor.at(i);
                //}else{
                    rewards.at(i) += pow(affinity, neighborDist) * rewardsNeighbor.at(i);
                //}
            }
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
                FRRs.at(_subProblem).at(op) = decays.at(op) / decaySum;
            else
                FRRs.at(_subProblem).at(op) = 0.;
        }
    }

    /**
     * Return best next op for subProblem FFRMAB_NR version
     *
     * take consideration of neighborhood information
     *
     * @param _subProblem
     * @return
     */
    unsigned getBestOp(unsigned _subProblem) {

        unsigned selectedOp;

        // check if all operator are used at least once
        if (checkUnusedOp.at(_subProblem)) {

            std::vector<int> unusedOpIndexes;

            for (unsigned i = 0; i < unusedOp.at(_subProblem).size(); i++) {
                if (unusedOp.at(_subProblem).at(i)) {
                    unusedOpIndexes.push_back(i);
                }
            }

            // all op will be used at least once after this iteration
            if (unusedOpIndexes.size() <= 1)
                checkUnusedOp.at(_subProblem) = false;

            int randIndex = rand() % unusedOpIndexes.size();

            // getting next op choose randomly
            selectedOp = unusedOpIndexes.at(randIndex);

            // set op as viewed
            unusedOp.at(_subProblem).at(selectedOp) = false;

        } else {


            // Use UCB to best next op
            double maxValue = -1.;

            // sum of number operator (sliding window size)
            int nopSum = 0;

            // selected operator will based only on local information
            auto fitnessRateRanks = FRRs.at(_subProblem);

            for (unsigned i = 0; i < nop.at(_subProblem).size(); ++i) {
                nopSum += nop.at(_subProblem).at(i);
            }

            // search best op at time t for subProblem
            for (unsigned i = 0; i < fitnessRateRanks.size(); i++) {

                double explorationValue = sqrt((2 * log(nopSum)) / nop.at(_subProblem).at(i));
                double currentValue = fitnessRateRanks.at(i) + (C * explorationValue);

                if (currentValue >= maxValue) {

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
};// end FFRMAB_DK
#endif
