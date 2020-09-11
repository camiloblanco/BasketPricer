/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/

#include "EurBasketCall.h"

/****************************************************************************************
*									MEMBER FUNCTIONS									*
****************************************************************************************/

//constructors
EurBasketCall::EurBasketCall() : BasketOptionBS () { m_type = "call"; }
EurBasketCall::EurBasketCall(double T, double K) : BasketOptionBS (T, K) { m_type = "call"; }

// Arimetic average Pay Off function
double EurBasketCall::payOff(vector <double> stockPrices) {

    // Calculate the arimetic average of simutaled stock prices
    double S = accumulate(stockPrices.begin(), stockPrices.end(), 0.0) / stockPrices.size();
    
    // Return the payoff by comparing the share price and the strike
    return (S > m_K) ? (S - m_K) : 0;
}

// Boundary condition calculation function
double EurBasketCall::boundaryCond(MarketBS market, vector <double> stockPrices, double time) {

    // Calculate the arimetic average of boundary stock prices
    double average = accumulate(stockPrices.begin(), stockPrices.end(), 0.0) / stockPrices.size();
    // Calculate the boundary value and return
    double  boundaryValue = average - m_K * exp(-market.get_r()*(m_T-time));
    return (boundaryValue >0) ? boundaryValue : 0;
}