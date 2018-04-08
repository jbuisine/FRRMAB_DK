#include <iostream>
#include <fstream>
#include <operators/crossover.h>

#include "evals/moUBQPEval.h"
#include "operators/mutation.h"
#include "moead/subProblems.h"
#include "moead/moFRRMAB_DK.h"
#include "moead/hyperVolume.h"
#include "moead/paretoFront.h"
#include "moead/init.h"


//#include "mpicxx.h"
//#include "mpi.h"

using namespace std;
/***
 * Main
 *
 *
 ***/
int main(int argc, char ** argv) {

    /**
     * sizes <- c(500, 1000, 2000, 3000, 4000, 5000)
     * correlations <- c(-0.5, -0.2, 0, 0.2, 0.5)
     * correlationsStr <- c("neg05", "neg02", "0", "pos02", "pos05")
     */

    // file information
    const std::string ubqpInstance = "mubqpInstance";
    const std::string fileSep = "_";
    const std::string fileInstExt = ".dat";
    const std::string fileStatsExt = ".txt";

    //std::vector<std::string> instSizes = {"500", "1000", "2000", "3000", "4000", "5000"};
    //std::vector<std::string> correlationsStr = {"neg05", "neg02", "0", "pos02", "pos05"};
    //std::vector<double> affinities = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
    //std::vector<std::string> affinitiesStr = { "01", "02", "03", "04", "05", "06", "07", "08", "09" };

    std::vector<std::string> instSizes = {"500", "1000", "2000"};
    std::vector<std::string> correlationsStr = {"neg05", "neg02", "0", "pos02", "pos05"};
    std::vector<double> affinities = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
    std::vector<std::string> affinitiesStr = { "01", "02", "03", "04", "05", "06", "07", "08", "09" };


    // static parameters
    unsigned mu = 100;
    unsigned T = 20;
    unsigned W = 40;
    double C = sqrt(2);
    double D = 0.5;
    unsigned nbEval = 10000;
    unsigned seed = 10;

    const std::string ubqpFolder = "./../resources/ubqp/";
    const std::string ubqpStatsFolder = "./../resources/ubqp/stats/";

    const std::string globalStats = ubqpStatsFolder + "globalStats_DK.csv";

    // random seed
    srand(seed);


    FILE *globalStatsFile = fopen(globalStats.c_str(), "w");
    fprintf(globalStatsFile, "algo; instSize; corrInst; HV; time; alpha;\n");
    fflush(globalStatsFile);

    ///////////////
    // FRRMAB_DK //
    ///////////////

    for (int i = 0; i < instSizes.size(); ++i) {

        for (int j = 0; j < correlationsStr.size(); ++j) {

            // get filename of instance
            std::string dataFileName = ubqpFolder + ubqpInstance + fileSep + instSizes[i] + fileSep + correlationsStr[j] + fileInstExt;
            std::string UBQPInstanceName = ubqpInstance + fileSep + instSizes[i] + fileSep + correlationsStr[j] + fileInstExt;

            // init all context info
            UBQPParser fparser(dataFileName);

            int problem_size = fparser.getN();

            // Evaluation function
            UBQPEval eval(fparser);

            InitUBQP init;

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


            // init decomposition with WeightedSum mono objective function
            WeightedSumSubProblems sp(mu, 0.0, 0.0, T, W);

            HillClimber repair(100, eval, sp, mutation1);

            for (int k = 0; k < affinities.size(); ++k) {

                sp.print();

                std::string fileout =  ubqpStatsFolder + std::string("output_DK") + fileSep + ubqpInstance + fileSep + instSizes[i] + fileSep + correlationsStr[j] + fileSep + "a" + affinitiesStr[k] + fileStatsExt;
                std::string pfFileout =  ubqpStatsFolder + std::string("output_DK_PF") + fileSep + ubqpInstance + fileSep + instSizes[i] + fileSep + correlationsStr[j] + fileSep + "a" + affinitiesStr[k] + fileStatsExt;

                cout << "----Starting FRRMAB_DK with UBQP " << UBQPInstanceName << " instance with alpha = " << affinities[k] << "----" << endl;

                // getting time of start algo
                clock_t tStart = clock();

                FRRMAB_DK algo(eval, sp, false, init, operators, repair, mu, C, D, affinities[k], nbEval);

                algo.run(fileout.c_str());

                // getting pareto front from population
                std::vector<moSolution> pf = algo.pfPop;

                ofstream file;
                file.open (pfFileout, ios::out);
                for(unsigned i = 0; i < pf.size(); i++){
                    file << pf[i].toString() << endl;
                }
                file.close();

                // Stats hyper volume
                HyperVolume hv;

                double hvScore = hv(pf, algo.minRefPoint);
                double timeTaken = (double)(clock() - tStart)/CLOCKS_PER_SEC;

                std::cout << "End of FRRMAB_DK (n. eval = " << nbEval << ", duration = " << algo.duration << ")" << std::endl;
                std::cout << "HV : " << hvScore << std::endl;

                /* Do your stuff here */
                std::cout << "Time taken : " << timeTaken << std::endl;

                fprintf(globalStatsFile, "FRRMAB_DK; %s; %s; %f; %f; %f;\n", instSizes.at(i).c_str(), correlationsStr.at(j).c_str(), hvScore, timeTaken, affinities.at(k));
                fflush(globalStatsFile);
            }
        }
    }

    return 0;
}
