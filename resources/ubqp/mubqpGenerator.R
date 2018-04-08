#!/usr/bin/env Rscript

#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License as published by the Free Software Foundation; version 3 
#     of the License.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Contact: http://mocolib.sourceforge.net
#
# Authors:
#     Arnaud Liefooghe <arnaud.lieffoghe@lifl.fr>
#     Sebastien Verel  <sebastien.verel@inria.fr>

# Syntax of the command line:
# ./mubqpGenerator.R rho M N density seed instance.dat
# Example:
# ./mubqpGenerator.R 0.8 2 18 2 0 instance.dat
#
# Or for some systems:
# R --slave --no-restore --file=mubqpGenerator.R --args 0.8 2 18 0.8 0 instance.dat
#args <- commandArgs(TRUE)

# parameter: correlation between matrices elements
#rho <- as.numeric(args[1]) 

# parameter: number of objectve functions
#M <- as.numeric(args[2])

# parameter: size of the bit string
#N <- as.numeric(args[3])

# parameter: density of the matrix (frequency of non-zero numbers)
#d <- as.numeric(args[4])

# parameter: seed number used by the generator
#s <- as.numeric(args[5])

# parameter: output file name 
#fileName <- args[6]

# Set current dir by default
this.dir <- dirname(parent.frame(2)$ofile)
setwd(this.dir)

library(MASS)


#####
## function to generate the instance
#####
generateMUBQP <- function(rho, M, N, d, s, fileName) {
  ## seed the random generator number
  set.seed(s)
  
  ## bounds of the linear coefficents
  
  lb_linear = -100
  ub_linear =  100
  
  ## bounds of the quadratic coefficents
  
  lb_quadratic = -100
  ub_quadratic =  100
  
  ## generation of the matrices
  
  # generation of matrix correlation between uniform random variables
  C       <- matrix(rep(rho,M), M, M)
  diag(C) <- rep(1, M)
  
  R <- 2 * sin(pi / 6 * C)
  
  # pdf of multivariate normal law
  index <- 1
  indexMax <- 1000
  fim <- pnorm(mvrnorm(n=indexMax, mu = rep(0, M), Sigma = R))
  
  ## print the function
  
  f <- file(fileName, open = "w")
  
  # heading of the file
  cat("p MUBQP", rho, M, N, d,"\n", file = f)
  
  # matrix for each dimension (only display size of matrix)
  cat(N, "\n", file = f)
  
  for( i in seq(1, N * N) ) {
    zero = runif(1)
    for( m in seq(1,M) )
      if (zero < d) { # apply the density parameter on the matrix
        if ((i-1) %% N == (i-1) %/% N) # linear part of the matrix ?
          cat(round(lb_linear + (ub_linear - lb_linear) * fim[index,m]), " ", file = f)
        else
          cat(round(lb_quadratic + (ub_quadratic - lb_quadratic) * fim[index,m]), " ", file = f)
      }
    else
      cat(0, " ", file = f)
    cat("\n", file = f)
    
    index <- index + 1
    if (indexMax < index) {  # generate indexMax random numbers to reduce the computational time
      index <- 1
      fim <- pnorm(mvrnorm(n=indexMax, mu = rep(0, M), Sigma = R))
    } 	   
  }
  
  close(f)
}

sizes <- c(500, 1000, 2000, 3000, 4000, 5000)
correlations <- c(-0.5, -0.2, 0, 0.2, 0.5)
correlationsStr <- c("neg05", "neg02", "0", "pos02", "pos05")
density = 0.8
nbObj = 2


generateAll <- function(){
  for(s in sizes){
    
    for(c in correlations){
      
      index <- which(correlations == c)
      
      filename =  paste0(paste0(paste0(paste0("mubqpInstance_", s), "_"), correlationsStr[index]), ".dat")
      print(filename)
      
      generateMUBQP(c, nbObj, s, density, 32, filename)
    }
  }
}



# execution of the generator
#generateMUBQP(rho, M, N, d, s, fileName)