//
// Created by jbuisine on 3/13/18.
//

#ifndef FRRMAB_DK_OPERATOR_H
#define FRRMAB_DK_OPERATOR_H

#include <solutions/moSolution.h>

// Op kind enum
enum class OpKind { MUTATION = 0, CROSSOVER };
std::ostream& operator<<(std::ostream& _out, const OpKind _value){
    switch (_value){
        case OpKind::MUTATION:
            _out << "Mutation operator";
            break;
        case OpKind::CROSSOVER:
            _out << "Crossover operator";
            break;
    }
}

class Operator {
public:

    Operator(int _size, OpKind _kind) : size(_size), kind(_kind) {};

    virtual moSolution operator()(moSolution _solution) = 0;
    virtual moSolution operator()(moSolution _solution1, moSolution _solution2) {};

    int getSize() const {
        return size;
    }

    OpKind getKind() const{
        return kind;
    }

protected:

    OpKind kind;
    int size;
};


#endif //FRRMAB_DK_OPERATOR_H
