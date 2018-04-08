/**
   Cleaner version of MOEA/D

   Date: 2017/03/23

 **/
#include <iostream>
#include <fstream>
#include <operators/crossover.h>

#include "evals/moUBQPEval.h"
#include "operators/mutation.h"
#include "moead/subProblems.h"
#include "moead/moFRRMAB.h"
#include "moead/hyperVolume.h"
#include "moead/paretoFront.h"
#include "moead/init.h"


//#include "mpicxx.h"
//#include "mpi.h"

using namespace std;
/***
 * Main
 *
 ***/
int main(int argc, char ** argv) {

    // getting context files
    std::string dataFileName = std::string("./../resources/ubqp/") + argv[1];
    unsigned mu = atoi(argv[2]);
    unsigned T = atoi(argv[3]);
    unsigned W = atoi(argv[4]);
    double C = atof(argv[5]);
    double D = atof(argv[6]);
    unsigned nbEval = atoi(argv[7]);
    unsigned seed = atoi(argv[8]);
    std::string fileout = std::string("./../resources/ubqp/stats/") + argv[9];
    std::string pfFileout = std::string("./../resources/ubqp/stats/") + argv[10];

    // init all context info
    UBQPParser fparser(dataFileName);

    int problem_size = fparser.getN();

    // Evaluation function
    UBQPEval eval(fparser);

    // random seed
    srand(seed);

    // Set Operators
    std::vector<Operator*> operators;

    StandardRndMutation mutation1(problem_size);
    StandardDoubleRndMutation mutation2(problem_size);
    StandardTripleRndMutation mutation3(problem_size);
    StandardCrossover crossover1(problem_size);
    StandardRndCrossover crossover2(problem_size);

    operators.push_back(&mutation1);
    operators.push_back(&mutation2);
    //operators.push_back(&mutation3);
    //operators.push_back(&crossover1);
    operators.push_back(&crossover2);

    // End set Operators

    InitUBQP init;

    // init decomposition with WeightedSum mono objective function
    WeightedSumSubProblems sp(mu, 0.0, 0.0, T, W);

    HillClimber repair(100, eval, sp, mutation1);

    sp.print();

    cout << "----Starting FRRMAB with UBQP instance----" << endl;
    FRRMAB algo(eval, sp, false, init, operators, repair, mu, C, D, nbEval);

    algo.run(fileout.c_str());

    // getting pareto front from population
    std::vector<moSolution> pf = algo.pfPop;

    ofstream file;

    file.open (pfFileout, ios::out);
    for(unsigned i = 0; i < pf.size(); i++){
        pf[i].print();
        file << pf[i].toString() << endl;
    }
    file.close();

    // Stats hyper volume
    HyperVolume hv;

    std::cout << "End of FRRMAB (n. eval = " << nbEval << ", duration = " << algo.duration << ")" << std::endl;
    std::cout << "HV : " << hv(pf, algo.minRefPoint) << std::endl;
  
    return 0;
}
