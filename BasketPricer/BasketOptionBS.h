/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/
#pragma once
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <random>
#include <numeric>
#include <functional>
#include <Eigen/Dense>
#include "MarketBS.h"

using namespace std;
using namespace Eigen;

/****************************************************************************************
*									CLASS DECLARATION									*
****************************************************************************************/
class BasketOptionBS
{
public:   
    //constructors
    BasketOptionBS();
    BasketOptionBS(double T, double K);

    //accessors
    void setT(double T);
    void setK(double K);
    double getT();
    double getK();
    string getType();

    // Public Member functions
    double  priceByMC(MarketBS market, int N, double & price, double& error);

    double  priceByEFD(MarketBS market,int timeSteps, int sSteps, double& price);

    //destructors
    ~BasketOptionBS();

protected:
    // protected Member functions
    virtual double payOff(vector <double> stockPrices)=0;
    virtual double boundaryCond(MarketBS market, vector <double> stockPrices, double time) = 0;

    void printTimefixed(vector<vector<vector<double>>> Lattice3D, int timeStep);
    void printS1fixed(vector<vector<vector<double>>> Lattice3D, int s1Step);
    void printS2fixed(vector<vector<vector<double>>> Lattice3D, int s2Step);

    // protected Member variables
    double m_T;         // Time to maturity in years (T)
    double m_K;         // Strike (Exercise) Price (K)
    string m_type;      // String for option type
};

