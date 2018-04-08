//
// Created by jbuisine on 12/01/18.
//

#ifndef _MOALGO_H
#define _MOALGO_H

class MultiObjectiveAlgo {
public:
    std::vector<moSolution> pfPop;
    std::pair<double, double> minRefPoint;
    unsigned maxEval;
    unsigned duration;
    /**
    * FFRMAB implementation
    * @param fileout
    */
    virtual void run(const char *fileout) = 0;

protected:
    std::vector<moSolution> pop;

    void updateMinRefPoint(moSolution _sol){

        double minX = minRefPoint.first;
        double minY = minRefPoint.second;

        if(_sol.objvec[0] < minX){
            minX = _sol.objvec[0];
        }

        if(_sol.objvec[1] < minY){
            minY = _sol.objvec[1];
        }

        minRefPoint = std::make_pair(minX, minY);
    }
};


#endif //_MOALGO_H
