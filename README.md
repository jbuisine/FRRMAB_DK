# FRRMAB_DK

## Description

This project wants to introduce a new approach in use of UCB Multi Armed Bandits algorithm into the  ***Multiobjective Evolutionnary Algorithm based on Decomposition*** (MOEA/D). 
This MAB algorithm brings an automatic selection of an operator (mutation/crossover ) used to generate new solution (offspring) in order to reduce time consuming and lead to best solutions of the search space.
The MOEA/D algorithm proposes scalar decomposition of multi objectives problem into ***n*** mono-objective problems. 
The final approach of this project is to introduce a new way of using MAB algorithm on each sub problem with information shares based on affinity of sub problems. FRRMAB_DK is based on FRRMAB which have been proposed **[[1]](https://doi.org/10.1109/TEVC.2013.2239648)**.

## Installation

It's a cmake project, so you only need to :

```commandline
mkdir build 
```

```commandline
cd build 
```

And then : 
```commandline
cmake 
```

# MOEA/D

For MOEA/D algorithm UBQP instances are used and generated following this available [generator](http://mocobench.sourceforge.net/index.php?n=Problem.MUBQP).

You can use **resources/ubqp/mubqpGenerator.R** script with specific parameters to generate instances.

# Licence

[LGPLv3](http://www.cecill.info/index.en.html)
