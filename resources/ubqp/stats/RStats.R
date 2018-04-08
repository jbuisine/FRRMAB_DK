library(ggplot2)

this.dir <- dirname(parent.frame(2)$ofile)
setwd(this.dir)

readFile <- function(filename, s){
  return(read.csv2(header = TRUE, file = filename, sep = s))
}

# dfStats <- readFile("output.txt", sep = " ")
# dfStatsDK <- readFile("output_dk.txt", sep = " ")
# dfPF_FRRMAB <- readFile("front_pa_frrmab.txt", sep = " ")
#dfPF_FRRMAB_DK <- readFile("front_dk.txt", " ")
#currentPF <- readFile("currentPF.txt", sep = " ")

# Erase sol index info
# dfPF_FRRMAB <- dfPF_FRRMAB[, 1:3]
#dfPF_FRRMAB_DK <- dfPF_FRRMAB_DK[, 1:3]
#currentPF <- currentPF[, 1:3]

# head of dfStats
# names(dfStats) <- c("ID", "Fitness", "Obj1", "Obj2", "DIR", "FROM", "BEST", "OP")
# names(dfStatsDK) <- c("ID", "Fitness", "Obj1", "Obj2", "DIR", "FROM", "BEST", "OP")
# names(dfPF_FRRMAB) <- c("Fitness", "Obj1", "Obj2")
#names(dfPF_FRRMAB_DK) <- c("Fitness", "Obj1", "Obj2")
#names(currentPF) <- c("Fitness", "Obj1", "Obj2")

drawOpDistrubution <- function(df){
  
  repOp <- prop.table(table(df$OP[df$OP >= 0]))
  barplot(repOp, 
          main="Operators' distribution", 
          xlab="Index of op",
          ylim = c(0,1))
}

drawPF <- function(df){
  
  ggplot() + 
    geom_point(data=df, aes(x=Obj1, y=Obj2), color='blue') + geom_smooth(method = 'loess') 
}

# drawOpDistrubution(dfStats)
# drawOpDistrubution(dfStatsDK)
# 
# drawPF(dfPF_FRRMAB)
#drawPF(dfPF_FRRMAB_DK)
#drawPF(currentPF)

extractDataHV <- function(df, filename, limit){
  
  f <- file(filename, open = "w")
  
  for(i in 1:nrow(df)) {
    
    row <- df[i,]
    cat(paste0(paste0("& ", signif(as.numeric(format(row[4], scientific = TRUE)), 3)), "\n"), file = f)
    
    if(i %% limit == 0){
      cat("\n", file = f)
    }
  }
  
  close(f)
}

extractDataTime <- function(df, filename, limit){
  
  f <- file(filename, open = "w")
  
  for(i in 1:nrow(df)) {
    
    row <- df[i,]
    cat(paste0(paste0("& ", signif(as.numeric(format(row[5], scientific = TRUE)), 3)), "\n"), file = f)
    
    if(i %% limit == 0){
      cat("\n", file = f)
    }
  }
  
  close(f)
}


globalDKData = readFile("globalStats_DK.csv", ";")
globalData = readFile("globalStats.csv", ";")

extractDataHV(globalDKData, "dataOutputDK.tex", 9)
extractDataHV(globalData, "dataOutput.tex", 1)

extractDataTime(globalDKData, "dataOutputTimeDK.tex", 9)
extractDataTime(globalData, "dataOutputTime.tex", 1)



