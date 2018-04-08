//
// Created by jbuisine on 16/01/18.
//

#ifndef PARETOFRONT_H
#define PARETOFRONT_H

#include <algorithm>
#include "hyperVolume.h"

/**
 * Class which returns pareto front from population
 */
class ParetoFront{
public:
    std::vector<moSolution> operator()(std::vector<moSolution> _pop, bool _minimize){

        unsigned popSize = _pop.size();

        if(popSize <= 0){
            throw "Error, population is empty";
        }

        unsigned nbObjective = _pop[0].objvec.size();

        std::vector<moSolution> paFront = std::vector<moSolution>();
        std::vector<unsigned> indexes;

        // check if solution dominate all others solutions
        for (int i = 0; i < _pop.size(); ++i) {

            for (int j = 0; j < _pop.size(); ++j) {

                if((std::find(indexes.begin(), indexes.end(), j) != indexes.end())) {
                    continue;
                }

                unsigned nbDominatedObjective = 0;

                for (int k = 0; k < nbObjective; ++k) {

                    // check problem type (minimize or maximize)
                    if(_minimize){

                        if(_pop[i].objvec.at(k) > _pop[j].objvec.at(k)){
                            nbDominatedObjective++;
                        }
                    }else{

                        if(_pop[i].objvec.at(k) < _pop[j].objvec.at(k)){
                            nbDominatedObjective++;
                        }
                    }
                }

                // check if solution is non dominated
                if(nbDominatedObjective == nbObjective) {
                    indexes.push_back(i);
                    break;
                }
            }
        }

        // add non dominated solution found
        for (int i = 0; i < popSize; ++i) {

            if(!(std::find(indexes.begin(), indexes.end(), i) != indexes.end())) {
                paFront.push_back(_pop.at(i));
            }
        }

        return paFront;
    }
};

#endif //PHOTOALBUM_PARETOFRONT_H
