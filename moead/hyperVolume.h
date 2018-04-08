//
// Created by jbuisine on 11/01/18.
//

#ifndef PHOTOALBUM_HYPERVOLUME_H
#define PHOTOALBUM_HYPERVOLUME_H


struct moObj1Cmp{

    bool operator()(std::pair<double, double> &p1, std::pair<double, double> &p2){
        return p1.first > p2.first;
    };
};

struct moObj2Cmp{

    bool operator()(std::pair<double, double> &p1, std::pair<double, double> &p2){
        return p1.second > p2.second;
    };
};

class HyperVolume{

public:
    double operator()(std::vector<moSolution> _sols, std::pair<double, double> _ref ){

        // can't override < operator of moSolution(throw issue when = operator is also override)
        // use of pair
        std::vector<std::pair<double, double> > objs;

        for(int i = 0; i < _sols.size(); i++){
            objs.push_back(std::make_pair(_sols.at(i).objvec[0], _sols.at(i).objvec[1]));
        }

        // sort x axis value by desc
        std::sort(objs.begin(), objs.end(), moObj1Cmp());

        double totalVolume = 0.;
        double previousY;

        double minX = _ref.first;
        double minY = _ref.second;

        for(int i = 0; i < objs.size(); ++i) {

            double x = objs.at(i).first- minX;

            double y = objs.at(i).second - minY;

            if(i == 0){
                totalVolume += (x * y);
            }else{
                totalVolume += (x * y) - (previousY * x);
            }

            previousY = y;
        }

        return totalVolume;
    }
};


#endif //PHOTOALBUM_HYPERVOLUME_H
