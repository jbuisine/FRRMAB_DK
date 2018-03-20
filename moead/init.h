//
// Created by jbuisine on 04/12/17.
//

#ifndef QAP_INIT_H
#define QAP_INIT_H


/*
 * Standard Mutation operator with fixed mutation rates
 */
class Init {
public:
    /**
     * Constructor
     **/
    Init() {}

    virtual void operator()(moSolution & _solution) = 0;
};

class InitQAP : public Init{
public:
    /**
     * Constructor
     **/
    InitQAP() : Init() {}

    virtual void operator()(moSolution & _solution) {

        _solution.resize(_solution.size());

        for(int i = 0; i < _solution.size(); i++){
            _solution[i] = i;
        }

        std::random_shuffle(&_solution[0], &_solution[_solution.size()-1]);
    }
};

class InitUBQP : public Init{
public:
    /**
     * Constructor
     **/
    InitUBQP() : Init() {}

    virtual void operator()(moSolution & _solution) {

        _solution.resize(_solution.size());

        // bool sol..
        for(int i = 0; i < _solution.size(); i++){
            _solution[i] = rand() % 2;
        }

        std::random_shuffle(&_solution[0], &_solution[_solution.size()-1]);
    }
};


#endif //QAP_INIT_H
