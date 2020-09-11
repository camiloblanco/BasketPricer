/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/

#include "BasketEurPut.h"

/****************************************************************************************
*									MEMBER FUNCTIONS									*
****************************************************************************************/

//constructors
BasketEurPut::BasketEurPut() : BasketOptionBS () { m_type = "put"; }
BasketEurPut::BasketEurPut(double T, double K) : BasketOptionBS (T, K) { m_type = "put"; }

// Arimetic average Pay Off function
double BasketEurPut::payOff(vector <double> stockPrices) {

    // Calculate the arimetic average of simutaled stock prices
    double S = accumulate(stockPrices.begin(), stockPrices.end(), 0.0) / stockPrices.size();
    
    // Return the payoff by comparing the share pric and the strike
    return (m_K > S) ? (m_K - S) : 0;
}


// Boundary condition calculation function
double BasketEurPut::boundaryCond(MarketBS market, vector <double> stockPrices, double time) {

    // Calculate the arimetic average of boundary stock prices
    double average = accumulate(stockPrices.begin(), stockPrices.end(), 0.0) / stockPrices.size();
    // Calculate the boundary value and return
    double  boundaryValue = m_K * exp(-market.get_r() * (m_T - time))- average;
    return (boundaryValue > 0) ? boundaryValue : 0;
}