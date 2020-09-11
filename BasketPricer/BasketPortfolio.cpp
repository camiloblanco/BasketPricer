#include "EurPortfolio.h"

EurPortfolio::EurPortfolio()
{
}

void EurPortfolio::loadPortfolio(string fileName) {
	cout << "****************************************************************************" << endl;
	cout << "Loading the Dataset" << endl << endl;
	string line;
	ifstream myStream(fileName);
	if (myStream.is_open()) {
		int num = 1;
		//drop the first line
		getline(myStream, line);
		while (getline(myStream, line)) {
	
			//Parse the line by ',' into a vector of strings
			stringstream ss(line);
			vector<string> fields;
			while (ss.good()) {
				string substr;
				getline(ss, substr, ',');
				fields.push_back(substr);
			}
			double T =  stod(fields[0]);
			double K = stod(fields[1]); 
			double S0 = stod(fields[2]);
			double sigma = stod(fields[3]);
			double r = stod(fields[4]);
	
			if (T >= 0 && K >= 0 && S0 >= 0 && sigma >= 0 && r >= 0) {
				EurCallBS eurCallOption(T, K);
				EurPutBS eurPutOption(T, K);
				MarketBS marketBS(S0, sigma, r);
				m_callOptsVec.push_back(eurCallOption);
				m_putOptsVec.push_back(eurPutOption);
				m_marketsVec.push_back(marketBS);
			}
			else {
				cout << "Error in Member function  EurPortfolio::loadPortfolio" << endl;
				cout << "Parameters K ans T must be positive. " << endl;
			}
		}
		myStream.close();
		cout << "Dataset loaded" << endl << endl;
	}
	else
	{
		cout << "There was a problem opening the file: " << fileName << endl;
	}
}
void EurPortfolio::pricePortfolio(string fileName){
	cout << "****************************************************************************" << endl;
	cout << "Pricing the Dataset" << endl << endl;


	// Open fileName csv  file in new mode and save the header to clean the file
	ofstream fileStream(fileName);

	if (fileStream.is_open()) {
		// write the file headers :
		fileStream << "time" << "," << "strike_price" << "," << "stock_price" << "," << "volatility" << "," << "interest_rate" << ",";
		fileStream << "call_price" << "," << "put_price" << "," << "time_sec" << "," << endl;

		// Price portafolio with a loop for each asset
		for (int i = 0; i < m_marketsVec.size(); i++) {

			//Generate the dataset line with precision to 8 decimals

			fileStream << fixed << setprecision(8); 
			fileStream << m_callOptsVec[i].getT() << "," << m_callOptsVec[i].getK() << ",";
			double S0 = m_marketsVec[i].get_S0();
			double sigma = m_marketsVec[i].get_sigma();
			double r = m_marketsVec[i].get_r();
			fileStream << S0 << "," << sigma << "," << r  << ",";
			
			// Get starting timepoint 
			auto start = high_resolution_clock::now();
			// Price the option 
			fileStream << m_callOptsVec[i].priceByBSFormula(S0, sigma, r) << ",";
			fileStream << m_putOptsVec[i].priceByBSFormula(S0, sigma, r) << ",";
			// Get ending timepoint 
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<nanoseconds>(stop - start);
			double seconds = duration.count() / 1000000000.0;
			fileStream << seconds << endl;
		}
	}
	fileStream.close();

	cout << "End of Pricing the Dataset" << endl << endl;

}

EurPortfolio::~EurPortfolio()
{
	m_callOptsVec.clear();
	m_putOptsVec.clear();
	m_marketsVec.clear();
}

