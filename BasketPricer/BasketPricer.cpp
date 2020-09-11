/****************************************************************************************
* Project		:	Machine Learning and modern numerical techniques for high-dimensional
*					option pricing - Financial Computing MSc. Dissertation QMUL 2019/2020
* License		:	MIT License, https://opensource.org/licenses/MIT
* Copyright (c) :	2020 Camilo Blanco
* Mail - Web	:	mail@camiloblanco.com - www.camiloblanco.com
* Lenguaje / Env:	C++ / Microsoft Visual Studio Community 2019
* Git Control	:	
* Description	:	main CPP file for the numerical pricing algorithms
*
****************************************************************************************/

/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/
#include <iostream>
#include <iomanip>
#include <Eigen/Dense>
#include "MarketBS.h"
#include "BasketEurCall.h"
#include "BasketEurPut.h"

using namespace std;
using namespace Eigen;

/****************************************************************************************
*										FUNCTIONS				        				*
****************************************************************************************/

// Clear the console in any plataform
void clearConsole() {
#if defined _WIN32
	system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
	system("clear");
#elif defined (__APPLE__)
	system("clear");
#endif
}

// Pause execution waiting for a key
void menuPause() {
	cout << endl << "Enter any key and press enter to continue..." << endl;
	cin.ignore();
	cin.get();
}

// Function to read Martket data from console
MarketBS ReadMarketFromConsole() {
	double r = 0, input=0, cor_12 = 0;
	int n = 0;
	vector<double> S0;        // Initial Stock Prices (S0)
	vector<double> sigma;     // Annualized volatilities (sigma)

	cout << "Please enter the market annual risk-free interest rate (r): " << endl;
	cin >> r;

	cout << "Please enter the number of underlying assets (n): " << endl;
	cin >> n;

	for (int i = 0; i < n; i++) {
		cout << "Please enter the initial stock price (S0) for the asset number "<<i+1<<" : " << endl;
		cin >> input;
		S0.push_back(input);
	}
	for (int i = 0; i < n; i++) {
		cout << "Please enter the annualized volatility (sigma) for the asset number " << i + 1 << " : " << endl;
		cin >> input;
		sigma.push_back(input);
	}
	// Declare a dynamic matrix using the eigen library
	MatrixXd Cor(n, n);
	//Initialize diagonals
	Cor = MatrixXd::Identity(n,n);

	// Read the corelation and set up the Matrix
	if (n > 1) {
		for (int i = 0; i < n-1; i++) {
			for (int j = i + 1; j <= n-1; j++) {
				cout << "Please enter the correlation coeficient between assets numbers " << i+1 << " and " << j+1 << endl;
				cin >> input;
				Cor(i,j) = input;
				Cor(j,i) = input;
			}
		}
	}
	cout << "The correlation matrix is:" << endl << Cor << endl << endl;

	MarketBS market(n, S0, sigma, r, Cor);
	return market;
}

// Function to Price an European Basket Option using Monte Carlo method.
void EuropeanMC() {
	double T = 0, K = 0, N = 0;
	clearConsole();

	cout << "****************************************************************************" << endl;
	cout << " Price an european basket option using Monte Carlo on the Black-Scholes framework" << endl << endl;
	cout << "Please enter the time to maturity in years (T): " << endl;
	cin >> T;
	cout << "Please enter the strike price (K): " << endl;
	cin >> K;
	// Read the market data from console
	MarketBS market=ReadMarketFromConsole();

	cout << "Please enter the number of Monte Carlo simulations (N): " << endl;
	cin >> N;

	// Basic validations
	if (T > 0 && K >= 0 && N > 0) {

		// Declare a BasketOptionBS pointer vector and objects for the Call and Put options
		vector<BasketOptionBS*> optionsPTRvec;
		BasketOptionBS* eurCallPTR = new BasketEurCall(T, K);
		BasketOptionBS* eurPutPRT = new BasketEurPut(T, K);
		optionsPTRvec.push_back(eurCallPTR);
		optionsPTRvec.push_back(eurPutPRT);
	
		// Vairable declaration
		double price, error;

		//Loop over the BasketOptionBS pointer vector
		for (auto& option : optionsPTRvec) {
			option->setK(K);
			option->setT(T);
			// Price the options and display
			cout << endl << "The Monte Carlo results for the " << option->getType() << " option are:" << endl;
			cout << "The option price is: " << fixed << setprecision(6)<< option->priceByMC(market, N, price, error) << endl;
			cout << "The estimated standar error is : " << error << endl;
			cout << "As a % of the estimated price the error is : " <<  (error / price) * 100.0 << " %." << endl;
		}

		//Delete EurOptionBS Objects and clear the BasketOptionBS pointer vector
		for (auto pointer : optionsPTRvec)
		{
			delete pointer;
		}
		optionsPTRvec.clear();
	}
	else {
		cout << "All parameters must be positive. " << endl;
	}
	menuPause(); 
}

// Function to Price an European Basket Option using the Explicit Finite Diference method.
void EuropeanEFD() {
	double T = 0, K = 0, timeSteps = 100, sSteps = 10;
	clearConsole();

	cout << "****************************************************************************" << endl;
	cout << " Price an European basket option by the Explicit Finite Diferences on the Black-Scholes framework" << endl << endl;
	cout << "Please enter the time to maturity in years (T): " << endl;
	cin >> T;
	cout << "Please enter the strike price (K): " << endl;
	cin >> K;
	// Read the market data from console
	MarketBS market = ReadMarketFromConsole();

	cout << "Please enter the Number of time steps : " << endl;
	cin >> timeSteps;
	cout << "Please enter the Number of spot steps : " << endl;
	cin >> sSteps;

	if (T > 0 && K >= 0 && timeSteps > 0) {

		// Declare a BasketOptionBS pointer vector and objects for the Call and Put options
		vector<BasketOptionBS*> optionsPTRvec;
		BasketOptionBS* eurCallPTR = new BasketEurCall(T, K);
		BasketOptionBS* eurPutPRT = new BasketEurPut(T, K);
		optionsPTRvec.push_back(eurCallPTR);
		optionsPTRvec.push_back(eurPutPRT);

		// Vairable declaration
		double price, error;

		//Loop over the BasketOptionBS pointer vector
		for (auto& option : optionsPTRvec) {
			option->setK(K);
			option->setT(T);
			// Price the options and display
			cout << endl << "The EFD result for this " << option->getType() << " option is:" << endl;
			cout << " Option price is: " << fixed << setprecision(6) << option->priceByEFD(market, timeSteps, sSteps, price) << endl;
		}

		//Delete EurOptionBS Objects and clear the BasketOptionBS pointer vector
		for (auto pointer : optionsPTRvec)
		{
			delete pointer;
		}
		optionsPTRvec.clear();
	}
	else {
		cout << "All parameters must be positive. " << endl;
	}
	menuPause();
}


/****************************************************************************************
*							        MAIN FUNCTION				        				*
****************************************************************************************/

int main()
{
	int option = 9;

	while (option != 0) {
		//clearConsole();
		cout << "****************************************************************************" << endl;
		cout << "		Black-Scholes calculator and dataset generator		 " << endl << endl;
		cout << "Select an option by entering the given number:" << endl << endl;
		cout << "1. Price an european option using Monte Carlo on the BS framework" << endl;
		cout << "2. Price an european option using EFD on the BS framework" << endl;
		cout << "0. To exit the program" << endl;
		cout << "****************************************************************************" << endl;
		cout << endl << "Please enter the option number:" << endl;
		cin >> option;
		if (option == 1) {
			EuropeanMC();
		}
		else if (option == 2) {
			EuropeanEFD();
		}
		else if (option == 0) {
			cout << endl << "Thank you for using this program, have a nice day. " << endl << endl;
		}
		else {
			cout << "Invalid Option" << endl;
			menuPause();
		}
	}
}