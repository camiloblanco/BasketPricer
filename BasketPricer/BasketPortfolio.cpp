/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								                *
****************************************************************************************/

#include "BasketPortfolio.h"

/****************************************************************************************
*									MEMBER FUNCTIONS									                   *
****************************************************************************************/

BasketPortfolio::BasketPortfolio()
{
}

void BasketPortfolio::loadPortfolio(string fileName) {
	cout << "****************************************************************************" << endl;
	cout << "Loading the Dataset" << endl << endl;
	//Variable declaration
	string line;
	int n = 0;
	//Open the file
	ifstream myStream(fileName);
	if (myStream.is_open()) {
		int num = 1;
		//drop the first line
		getline(myStream, line);
		while (getline(myStream, line)) {
	
			//Parse the line by ',' into a vector of strings (fields)
			stringstream ss(line);
			vector<string> fields;
			while (ss.good()) {
				string substr;
				getline(ss, substr, ',');
				fields.push_back(substr);
			}
			n = 1;
			
			//extract the data to variables
			vector<double> S0;        // Initial Stock Prices (S0)
			vector<double> sigma;     // Annualized volatilities (sigma)

			double T =  stod(fields[0]);
			double K = stod(fields[1]); 
			double r = stod(fields[2]);
			//Validate the data, select the option type and store data in member vectors	
			if (T >= 0 && K >= 0 && r >= 0) {		

				for (int i = 0; i < n; i++) {
					S0.push_back(stod(fields[3+i]));
				}

				for (int i = 0; i < n; i++) {
					sigma.push_back(stod(fields[4+i]));
				}

				// Declare a dynamic matrix using the eigen library
				MatrixXd Cor(n, n);
				//Initialize diagonals
				Cor = MatrixXd::Identity(n, n);
				// Read the corelation and set up the Matrix
				int pos = 0;
				if (n > 1) {
					for (int i = 0; i < n - 1; i++) {
						for (int j = i + 1; j <= n - 1; j++) {
							Cor(i, j) = stod(fields[6 + pos]);
							Cor(j, i) = stod(fields[6 + pos]);
							pos++;
						}
					}
				}
				MarketBS market(n, S0, sigma, r, Cor);
				m_marketsVec.push_back(market);
				int call = stod(fields[5]);
				int put = stod(fields[6]);
				
				if (call == 1 && put == 0) {
					BasketOptionBS* eurCallPTR = new EurBasketCall(T, K);
					m_basketOptionsPtrVec.push_back(eurCallPTR);
				}
				else {
					BasketOptionBS* eurPutPTR = new EurBasketPut(T, K);
					m_basketOptionsPtrVec.push_back(eurPutPTR);
				}				
			}
			else {
				cout << "Error in Member function  BasketPortfolio::loadPortfolio" << endl;
				cout << "Parameters K, T and r must all be positive. " << endl;
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
void BasketPortfolio::pricePortfolio(string fileName){
	cout << "****************************************************************************" << endl;
	cout << "Pricing the Dataset" << endl << endl;

	// Open fileName csv  file in new mode and save the header to clean the file
	ofstream fileStream(fileName);

	if (fileStream.is_open()) {
		// write the file headers :
		fileStream << "time" << "," << "strike_price" << "," << "interest_rate" << "," << "stock_price" << "," << "volatility" << ",";
		fileStream << "call" << "," << "put" << "," << "mc_price" << "," << "mc_time_sec" << "," << "mc_std_err" << endl;

		// Price the portafolio with a auto loop
		int i = 0;
		for (auto& option : m_basketOptionsPtrVec) {

			//Generate the dataset line with precision to 8 decimals

			fileStream << fixed << setprecision(8); 
			fileStream << option->getT() << "," << option->getK() << ",";
			double S0 = m_marketsVec[i].get_S0(0);
			double sigma = m_marketsVec[i].get_sigma(0);
			double r = m_marketsVec[i].get_r();
			fileStream << r << "," << S0 << "," << sigma << "," ;
			
			// Vairable declaration
			double mc_price, mc_std_err;
			int N = 1000;

			// Get starting timepoint 
			auto start = high_resolution_clock::now();
			// Price the option 
			option->priceByMC(m_marketsVec[i], N, mc_price, mc_std_err);
			// Get ending timepoint 
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<nanoseconds>(stop - start);
			double mc_time_sec = duration.count() / 1000000000.0;
			fileStream << "1" << "," << "0" << "," << mc_price << "," << mc_time_sec << "," << mc_std_err << endl;
			i++;
		}
	}
	fileStream.close();

	cout << "End of Pricing the Dataset" << endl << endl;

}

BasketPortfolio::~BasketPortfolio()
{
	m_basketOptionsPtrVec.clear();
	m_marketsVec.clear();
}

