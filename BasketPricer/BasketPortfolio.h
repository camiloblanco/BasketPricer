/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/
#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <fstream>
#include <vector> 
#include <chrono>

#include "BasketOptionBS.h"
#include "EurBasketCall.h"
#include "EurBasketPut.h"
#include "MarketBS.h"

using namespace std;
using namespace std::chrono;

/****************************************************************************************
*									CLASS DECLARATION									*
****************************************************************************************/

class BasketPortfolio
{
public:
	BasketPortfolio();
	void loadPortfolio(string fileName);
	void pricePortfolioByMC(string fileName, int N);
	~BasketPortfolio();
private:
	vector<BasketOptionBS*> m_basketOptionsPtrVec;
	vector<MarketBS> m_marketsVec;
	vector<string> m_header;
	int m_nu;
};

