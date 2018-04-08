//
// Created by jbuisine on 15/11/17.
//

#ifndef UBQP_PARSER_H
#define UBQP_PARSER_H

#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

/**
 * Class to handle data of a QAP instance
 */

class UBQPParser
{
public:

    /**
     * Constructor
     *
     * @param _dataFile the name of the QAP data file
     */
    UBQPParser(const std::string _dataFile)
    {
        init(_dataFile);
    };

    /**
     * the number of photos
     */
    unsigned int getN()
    {
        return n;
    };

    /**
     * first similitude matrix
     */
    double** getBOne()
    {
        return BOne;
    };


    /**
     * second similitude matrix
     */
    double** getBTwo()
    {
        return BTwo;
    };


    /**
     * printing...
     */
    void printOn(std::ostream & _os) const
    {
        _os << "print data form Parser" << std::endl;
    };


private:

    // number of elements
    unsigned n;

    // first similitude matrix
    double ** BOne;

    // second similitude matrix
    double ** BTwo;

    /**
     * Initialisation of the parameters with the data photo and disposition files
     *
     * @param _dataFile the name of QAP instance data file
     */
    void init(const std::string _dataFile)
    {
        // read data file
        std::ifstream file(_dataFile, ios::in);

        std::string line;

        // default get first line information
        std::getline(file, line);
        std::cout << "Problem instance : " << line << std::endl;

        // getting size problem
        std::getline(file, line);
        n = stoi(line);

        writeMatrixData(file);

        file.close();
    };

    /**
     * Method which write into matrix problem data
     * @param _file
     * @param m
     */
    void writeMatrixData(std::ifstream& _file){

        // init matrix first dimension size
        BOne = new double*[n];
        BTwo = new double*[n];

        std::string line;

        unsigned nbLines = n * n;

        unsigned index = 0;

        for(int i = 0; i < nbLines; i++){

            unsigned j = i % n;

            if(j == 0){

                if(i != 0)
                    index++;

                BOne[index] = new double[n];
                BTwo[index] = new double[n];
            }

            // get next line data
            std::getline(_file, line);

            stringstream ss(line);

            // set vectors data
            std::vector<string> strs;

            std::string entry;

            while (ss >> entry)
                strs.push_back(entry);

            BOne[index][j] = stod(strs.at(0));
            BTwo[index][j] = stod(strs.at(1));
        }
    }
};

#endif //UBQP_PARSER_H
