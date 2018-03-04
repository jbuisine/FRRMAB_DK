/**
   Cleaner version of MOEA/D

   Date: 2017/03/23

 **/

#include <iostream>
#include <fstream>

#include "solutions/moUBQPSolution.h"
#include "evals/moUBQPEval.h"
#include "moead/mutation.h"
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
 * argv[1] : Data file path
 * argv[2] : Disposition file path
 * argv[3] : random seed
 * argv[4] : mu
 * argv[5] : T
 * argv[6] : W (sliding window size)
 * argv[7] : C (UCB scaling trade-off factor EvE)
 * argv[8] : D (decaying factor in [0,1])
 * argv[9] : time in second of the run
 * argv[10] : output file name
 *
 ***/
int main(int argc, char ** argv) {

    // getting context files
    std::string _dataFileName = "./../resources/ubqp/ubqpInstance.txt";

    // Get all params data
    //std::string _dataFileName = argv[1];
    //std::string _dispositionFileName = argv[1];
    //int seed = atoi(argv[3]);
    //unsigned mu = atoi(argv[4]);
    //unsigned T = atoi(argv[5]);
    //unsigned W = atoi(argv[6]);
    //double C = atof(argv[7]);
    //double D = atof(argv[8]);
    //unsigned neighborTaken = atoi(argv[9]);
    //double pFindNeighbor = atof(argv[10]);
    //unsigned duration = atoi(argv[11]);

    // default settings
    int seed = 10;
    unsigned mu = 100;
    unsigned T = 20;
    unsigned W = 15;
    double C = sqrt(2.);
    double D = 0.5;
    unsigned nbEval = 20000;

    // init all context info
    UBQPParser fparser(_dataFileName);

    int problem_size = fparser.getN();

    // Evaluation function
    UBQPEval eval(fparser);

    // random seed
    srand(seed);

    // Set Operators
    std::vector<Mutation*> mutations;

    StandardMutation mutation1(problem_size);
    StandardRndMutation mutation2(problem_size);
    DoubleStandardRndMutation mutation3(problem_size);
    TripleStandardRndMutation mutation4(problem_size);

    mutations.push_back(&mutation1);
    mutations.push_back(&mutation2);
    mutations.push_back(&mutation3);
    //mutations.push_back(&mutation4);

    // End set Operators

    InitUBQP init;

    // init decomposition with WeightedSum mono objective function
    WeightedSumSubProblems sp(mu, 0.0, 0.0, T, W);

    HillClimber repair(100, eval, sp, mutation1);

    sp.print();

    cout << "----Starting FRRMAB with UBQP instance----" << endl;
    FRRMAB algo(eval, sp, false, init, mutations, repair, mu, C, D, nbEval);

    char* fileout = "./../resources/qap/stats/output.txt"; //argv[12]

    algo.run(fileout);

    // getting pareto front from population
    std::vector<moSolution> pf = algo.pfPop;

    ofstream file;
    file.open ("./../resources/qap/stats/front_pa_frrmab_dk.txt", ios::out);
    for(unsigned i = 0; i < pf.size(); i++){
        file << pf[i].toString() << endl;
    }
    file.close();

    // Stats hyper volume
    HyperVolume hv;

    std::cout << "End of FRRMAB_NR (n. eval = " << nbEval << ", duration = " << algo.duration << ")" << std::endl;
    std::cout << "HV : " << hv(pf) << std::endl;
  
    return 0;
}