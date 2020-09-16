/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/
#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <chrono>
#include <Eigen/Dense>
#include "MarketBS.h"

using namespace std;
using namespace Eigen;

/****************************************************************************************
*									CLASS DECLARATION									*
****************************************************************************************/
class MarketBS
{
public:

    //constructors
    MarketBS();
    MarketBS(int n, vector<double> S0, vector<double> sigma, double r, MatrixXd CorM);

    //accessors
    double get_r();
    double get_n();
    vector<double> get_S0();
    double get_S0(int n);
    vector<double> get_sigma();
    double get_sigma(int n);
    double get_cor(int n, int m);


    // Public Member functions
    vector<double>  simulatePrices(double T);        //  For path independant   
    
    //destructors
    ~MarketBS();

private:

    // private Member variables
    int m_n;                    // Number of Assets in the market
    vector<double> m_S0;        // Initial Stock Prices (S0)
    vector<double> m_sigma;     // Annualized volatilities (sigma)
    double m_r;                 // Annual risk-free interest rate (r)
    MatrixXd m_CorM;            // Corretaltion Matrix
    MatrixXd m_ChoL;            // Cholesky factor L Matrix
};