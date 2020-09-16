/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								                *
****************************************************************************************/

#include "BasketPortfolio.h"

/****************************************************************************************
*									MEMBER FUNCTIONS									                   *
****************************************************************************************/

BasketPortfolio::BasketPortfolio()
{
	m_nu = 0;
}

void BasketPortfolio::loadPortfolio(string fileName) {
	cout << "****************************************************************************" << endl;
	cout << "Loading the Dataset" << endl << endl;
	//Variable declaration
	string line;
	//Open the file
	ifstream myStream(fileName);
	if (myStream.is_open()) {
		int num = 1;
		//read the first line
		getline(myStream, line);
		//detect the number of underlyings 8->1, 11->2, 15->3.
		//Parse the line by ',' into a vector of strings (fields)
		stringstream ssHeader(line);
		while (ssHeader.good()) {
			string substr;
			getline(ssHeader, substr, ',');
			m_header.push_back(substr);
		}
		if (m_header.size() == 8)
			m_nu = 1;
		else if (m_header.size() == 11)
			m_nu = 2;
		else if (m_header.size() == 15)
			m_nu = 3;
		cout << "Number of underlyings:" << m_nu << endl;

		while (getline(myStream, line)) {
	
			//Parse the line by ',' into a vector of strings (fields)
			stringstream ss(line);
			vector<string> fields;
			while (ss.good()) {
				string substr;
				getline(ss, substr, ',');
				fields.push_back(substr);
			}
			//cout << "Number of fields:" << fields.size() << endl;
			//extract the data to variables
			vector<double> S0;        // Initial Stock Prices (S0)
			vector<double> sigma;     // Annualized volatilities (sigma)

			double T =  stod(fields[0]);
			double K = stod(fields[1]); 
			double r = stod(fields[2]);
			int pos = 3;
			//Validate the data, select the option type and store data in member vectors	
			if (m_nu >0 && T >= 0 && K >= 0 && r >= 0) {
				
				//Read stock prices
				for (int i = 0; i < m_nu; i++) {
					S0.push_back(stod(fields[pos]));
					pos++;
				}
				//Read volatilities
				for (int i = 0; i < m_nu; i++) {
					sigma.push_back(stod(fields[pos]));
					pos++;
				}

				// Declare a dynamic matrix using the eigen library
				MatrixXd Cor(m_nu, m_nu);
				//Initialize diagonals
				Cor = MatrixXd::Identity(m_nu, m_nu);
				// Read the corelations and set up the Matrix
				if (m_nu > 1) {
					for (int i = 0; i < m_nu - 1; i++) {
						for (int j = i + 1; j <= m_nu - 1; j++) {
							Cor(i, j) = stod(fields[pos]);
							Cor(j, i) = stod(fields[pos]);
							pos++;
						}
					}
				}
				//cout << " The correlation matrix is:" << endl << Cor << endl << endl;
				MarketBS market(m_nu, S0, sigma, r, Cor);
				m_marketsVec.push_back(market);
				int call = stod(fields[pos]);
				int put = stod(fields[pos+1]);
				//cout << "Call: " << call << " and Put: " << put << endl;
				if (call == 1 && put == 0) {
					BasketOptionBS* eurCallPTR = new EurBasketCall(T, K);
					m_basketOptionsPtrVec.push_back(eurCallPTR);
					//cout << "Call option stored" << endl;
				}
				else {
					BasketOptionBS* eurPutPTR = new EurBasketPut(T, K);
					m_basketOptionsPtrVec.push_back(eurPutPTR);
					//cout << "Put option stored" << endl;
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
		cout << "There was some problem opening the file: " << fileName << endl;
	}
}
void BasketPortfolio::pricePortfolioByMC(string fileName, int N){

	cout << endl<<"Pricing the Dataset" << endl << endl;

	// Open fileName csv  file in new mode and save the header to clean the file
	ofstream fileStream(fileName);

	if (fileStream.is_open()) {
		// write the file headers :
		for (int i = 0; i < m_header.size()-1; i++) {
			fileStream << m_header[i] << ",";
		}
		fileStream << "mc_price" << "," << "mc_time_sec" << "," << "mc_std_err" << endl;
		// Price the portafolio with a auto loop
		int optCounter = 0;
		for (auto& option : m_basketOptionsPtrVec) {
			//Generate the dataset line with precision to 8 decimals
			fileStream << option->getT() << "," << option->getK() << ","<<m_marketsVec[optCounter].get_r() << ",";

			//Print to file stock prices
			for (int i = 0; i < m_nu; i++) {
				fileStream << m_marketsVec[optCounter].get_S0(i) << ",";
			}
			//Print to file volatilities
			for (int i = 0; i < m_nu; i++) {
				fileStream << m_marketsVec[optCounter].get_sigma(i) << ",";
			}

			// Print to file corelations
			if (m_nu > 1) {
				for (int i = 0; i < m_nu - 1; i++) {
					for (int j = i + 1; j <= m_nu - 1; j++) {
						fileStream << m_marketsVec[optCounter].get_cor(i,j)<< ",";
					}
				}
			}

			// Vairable declaration
			double mc_price, mc_std_err;
			fileStream << fixed << setprecision(8);
			// Get starting timepoint 
			auto start = high_resolution_clock::now();
			// Price the option 
			option->priceByMC(m_marketsVec[optCounter], N, mc_price, mc_std_err);
			// Get ending timepoint 
			auto stop = high_resolution_clock::now();
			auto duration = duration_cast<nanoseconds>(stop - start);
			double mc_time_sec = duration.count() / 1000000000.0;
			fileStream << option->getCall() << "," << option->getPut() << "," << mc_price << "," << mc_time_sec << "," << mc_std_err << endl;
			optCounter++;
		}
	}
	fileStream.close();
	cout << "End of Pricing the Dataset" << endl << endl;

}

BasketPortfolio::~BasketPortfolio()
{
	m_basketOptionsPtrVec.clear();
	m_marketsVec.clear();
	m_header.clear();
}

