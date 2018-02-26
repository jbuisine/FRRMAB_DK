# FRRMAB_DK

## Description


This project wants to introduce a new approach in use of UCB Multi Armed Bandits algorithm into the  ***Multiobjective Evolutionnary Algorithm based on Decomposition*** (MOEA/D). 
This MAB algorithm brings an automatic selection of an operator (mutation/crossover ) used to generate new solution (offspring) in order to reduce time consuming and lead to best solutions of the search space.
The MOEA/D algorithm proposes scalar decomposition of multi objectives problem into ***n*** mono-objective problems. 
The final approach of this project is to introduce a new way of using MAB algorithm on each sub problem with information shares based on affinity of sub problems.

#Installation

It's a cmake project, so you only need to :

```commandline
mkdir build cd build 
```

```commandline
cd build 
```

And then : 
```commandline
cmake 
```

## MOEA/D

For MOEA/D algorithm, QAP instances are generated following this available [generator](http://www.cs.bham.ac.uk/~jdk/mQAP/). 

## Licence

[CeCILL](http://www.cecill.info/index.en.html)
