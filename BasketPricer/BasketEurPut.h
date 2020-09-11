/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/
#pragma once
#include "BasketOptionBS.h"

/****************************************************************************************
*									CLASS DECLARATION									*
****************************************************************************************/

class BasketEurPut :	public BasketOptionBS
{
public:

    //constructors
    BasketEurPut();
    BasketEurPut(double T, double K);

    // Public Member functions
    double payOff(vector <double> stockPrices);
    double boundaryCond(MarketBS market, vector <double> stockPrices, double time);
};

