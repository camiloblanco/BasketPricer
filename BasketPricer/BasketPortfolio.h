#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <fstream>
#include <vector> 
#include <chrono>

#include "EurOptionBS.h"
#include "EurCallBS.h"
#include "EurPutBS.h"
#include "MarketBS.h"

using namespace std;
using namespace std::chrono;

class EurPortfolio
{
public:
	EurPortfolio();
	void loadPortfolio(string fileName);
	void pricePortfolio(string fileName);
	~EurPortfolio();
private:
	vector<EurCallBS> m_callOptsVec;
	vector<EurPutBS> m_putOptsVec;
	vector<MarketBS> m_marketsVec;
};

