//
// Created by jbuisine on 30/11/17.
//

#ifndef UBQP_EVAL_H
#define UBQP_EVAL_H

#include "moEval.h"
#include "parsers/UBQP_parser.hpp"

class UBQPEval : public moEval {
public:

    // constructor
    UBQPEval(UBQPParser _parser) : moEval() {

        // getting data
        BOne = _parser.getBOne();
        BTwo = _parser.getBTwo();
        pbSize = _parser.getN();
    }

    // evaluation function
    virtual void operator()(moSolution & _solution) {

        _solution.objvec.resize(2);

        _solution.objvec[0] = flowObj1(_solution);
        _solution.objvec[1] = flowObj2(_solution);
    }

private:

    // first flow matrix
    double ** BOne;

    // second flow matrix
    double ** BTwo;

    /**
     * computation of first objective
     * @param _sol the genotype to evaluate
     */
    double flowObj1(moSolution & _sol)
    {
        double score = 0.;

        for (int i = 0; i < pbSize; i++) {
            for (int j = 0; j < pbSize; j++) {
                score += BOne[i][j] * _sol[i] * _sol[j];
            }
        }

        return score;
    };

    /**
     * computation of second objective
     * @param _sol the genotype to evaluate
     */
    double flowObj2(moSolution & _sol)
    {
        double score = 0.;

        for (int i = 0; i < pbSize; i++) {
            for (int j = 0; j < pbSize; j++) {
                score += BTwo[i][j] * _sol[i] * _sol[j];
            }
        }

        return score;
    };
};


#endif // QAP_EVAL_H
