/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/
#pragma once
#include "BasketOptionBS.h"

/****************************************************************************************
*									CLASS DECLARATION									*
****************************************************************************************/

class EurBasketCall :	public BasketOptionBS
{
public:

    //constructors
    EurBasketCall();
    EurBasketCall(double T, double K);

    // Public Member functions
    double payOff(vector <double> stockPrices);
    double boundaryCond(MarketBS market, vector <double> stockPrices, double time);
};

