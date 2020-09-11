/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/
#pragma once
#include "BasketOptionBS.h"

/****************************************************************************************
*									CLASS DECLARATION									*
****************************************************************************************/

class EurBasketPut :	public BasketOptionBS
{
public:

    //constructors
    EurBasketPut();
    EurBasketPut(double T, double K);

    // Public Member functions
    double payOff(vector <double> stockPrices);
    double boundaryCond(MarketBS market, vector <double> stockPrices, double time);
};

