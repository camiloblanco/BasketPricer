/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/
#pragma once
#include "BasketOptionBS.h"

/****************************************************************************************
*									CLASS DECLARATION									*
****************************************************************************************/

class BasketEurCall :	public BasketOptionBS
{
public:

    //constructors
    BasketEurCall();
    BasketEurCall(double T, double K);

    // Public Member functions
    double payOff(vector <double> stockPrices);
    double boundaryCond(MarketBS market, vector <double> stockPrices, double time);
};

