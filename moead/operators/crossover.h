#ifndef __crossover__h
#define __crossover__h

#include "operators/operator.h"
#include <random>


class Crossover : public Operator {

public:
    /**
     * Constructor
     **/
    Crossover(int _size) : Operator(_size, OpKind::CROSSOVER) {}


    virtual moSolution operator()(moSolution _solution) {};
    virtual moSolution operator()(moSolution _solution1, moSolution _solution2) = 0;
};

/*
 * Standard random crossover operator
 */
class StandardCrossover : public Crossover {
public:
  /**
   * Constructor
   **/
  StandardCrossover(int _size) : Crossover(_size) {}

  // mutation operator
  virtual moSolution operator()(moSolution _solution1, moSolution _solution2){

      unsigned index;

      do{
          index = static_cast<unsigned int>(rand() % size);
      }while (index == 0 || index == (size -1));

      moSolution newSolution = _solution1;

      for (unsigned i = 0; i < size; i++){

          if(i <= index){

              newSolution[i] = _solution1[i];
          }else{

              newSolution[i] = _solution2[i];
          }
      }

      return newSolution;
  }
};

class StandardRndCrossover : public Crossover {
public:
    /**
     * Constructor
     **/
    StandardRndCrossover(int _size) : Crossover(_size) {}

    // mutation operator
    virtual moSolution operator()(moSolution _solution1, moSolution _solution2){

        unsigned index;

        do{
            index = static_cast<unsigned int>(rand() % size);
        }while (index == 0 || index == (size -1));

        moSolution newSolution = _solution1;


        double p = (double)rand() / RAND_MAX;

        for (unsigned i = 0; i < size; i++){

            if(i <= index){

                if(p <= 0.5)
                    newSolution[i] = _solution1[i];
                else
                    newSolution[i] = _solution2[i];
            }else{

                if(p > 0.5)
                    newSolution[i] = _solution1[i];
                else
                    newSolution[i] = _solution2[i];
            }
        }

        return newSolution;
    }
};

#endif
