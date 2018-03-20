#ifndef __mutation__h
#define __mutation__h

#include "operators/operator.h"
#include <random>


class Mutation : public Operator {

public:
    Mutation(int _size) : Operator(_size, OpKind::MUTATION) {}


    virtual moSolution operator()(moSolution _solution) = 0;
    virtual moSolution operator()(moSolution _solution1, moSolution _solution2) {};
};

/*
 * Standard random Mutation operator with fixed mutation rates
 */
class StandardRndMutation : public Mutation {
public:
  /**
   * Constructor
   **/
  StandardRndMutation(int _size) : Mutation(_size) {}

    // mutation operator
    virtual moSolution operator()(moSolution _solution) {

        unsigned index = static_cast<unsigned int>(rand() % size);

        _solution[index] = !_solution[index];

        _solution.from(_solution.ID());
        _solution.best(0);

        return _solution;
    }
};

/*
 * Standard Double Mutation operator with fixed mutation rates
 */
class StandardDoubleRndMutation : public Mutation {
public:

    /**
     * Constructor
     **/
    StandardDoubleRndMutation(int _size) : Mutation(_size) {}

    // mutation operator
    virtual moSolution operator()(moSolution _solution) {

        for(unsigned i = 0; i < 2; i++){

            unsigned index = static_cast<unsigned int>(rand() % size);

            _solution[index] = !_solution[index];
        }

        _solution.from(_solution.ID());
        _solution.best(0);

        return _solution;
    }
};


/*
 * Standard Double Mutation operator with fixed mutation rates
 */
class StandardTripleRndMutation : public Mutation {
public:

    /**
     * Constructor
     **/
    StandardTripleRndMutation(int _size) : Mutation(_size) {}

    // mutation operator
    virtual moSolution operator()(moSolution _solution) {

        for(unsigned i = 0; i < 3; i++){

            unsigned index = static_cast<unsigned int>(rand() % size);

            _solution[index] = !_solution[index];
        }

        _solution.from(_solution.ID());
        _solution.best(0);

        return _solution;
    }
};

#endif
