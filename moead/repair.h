//
// Created by jbuisine on 18/12/17.
//

#ifndef REPAIR_H
#define REPAIR_H

#include "subProblems.h"
#include "operators/mutation.h"
#include "evals/moEval.h"


class Repair{

public:
    Repair(unsigned _nbIteration, moEval &_evaluation, SubProblems &_subProblems, Mutation &_mutation)
            : subProblems(_subProblems), evaluation(_evaluation), mutation(_mutation){
        this->nbIteration = _nbIteration;
    };

    virtual moSolution operator()(moSolution _solution, unsigned _subProblem, bool _minimize) = 0;

protected:

    // number of iteration of local search
    unsigned nbIteration;

    SubProblems &subProblems;
    Mutation &mutation;
    moEval &evaluation;

};

class HillClimber : public Repair{

public:

    /**
     * Constructor
     **/
    HillClimber(unsigned _nbIteration, moEval &_evaluation, SubProblems &_subProblems, Mutation &_mutation)
            : Repair(_nbIteration, _evaluation, _subProblems, _mutation) {};


    virtual moSolution operator()(moSolution _solution, unsigned _subProblem, bool _minimize){

        unsigned iteration = 0;
        double bestFitness = _solution.fitness();
        moSolution bestSolution = _solution;

        unsigned size = _solution.size();
        while(iteration < nbIteration){

            moSolution currentSolution = bestSolution;
            currentSolution = mutation(currentSolution);

            for(int i = 0; i < size; i++){

                moSolution mutant = currentSolution;

                mutant = mutation(mutant);
                evaluation(mutant);

                double fitness = subProblems.scalarfunc(_subProblem, mutant);

                // choose kind of problem
                if(_minimize){
                    if(fitness < bestFitness){
                        bestFitness = fitness;
                        bestSolution = mutant;
                    }
                }else{
                    if(fitness > bestFitness){
                        bestFitness = fitness;
                        bestSolution = mutant;
                    }
                }

                iteration++;
            }
        }

        // set best solution
        return bestSolution;
    }
};

#endif
