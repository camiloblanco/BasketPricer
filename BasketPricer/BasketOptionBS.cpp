/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								                *
****************************************************************************************/
#include "BasketOptionBS.h"

/****************************************************************************************
*									MEMBER FUNCTIONS									                   *
****************************************************************************************/


//constructors
BasketOptionBS::BasketOptionBS() :m_T(0.0), m_K(0.0), m_type("") { }
BasketOptionBS::BasketOptionBS(double T, double K) : m_T(T), m_K(K), m_type("") { }

//accessors
void BasketOptionBS::setT(double T) { m_T = T; }
void BasketOptionBS::setK(double K) { m_K = K; }
double BasketOptionBS::getT() { return m_T; }
double BasketOptionBS::getK() { return m_K; }
int BasketOptionBS::getCall() { return m_type.compare("call") == 0 ? 1 : 0; }
int BasketOptionBS :: getPut(){ return m_type.compare("put") == 0 ? 1 : 0; }
string BasketOptionBS::getType() { return m_type; }


// Pricing function using Monte Carlo Method
double  BasketOptionBS::priceByMC(MarketBS market, int N, double& price, double& error){

    vector<double> payOffs(N);

    // Simulate N paths, and store the resulting Payoffs in the samples vector's elements
    for (auto& sample : payOffs)
    {   
        //Simulate a path for european options and modify the coresponding payOffs vector element
        vector<double> simulations = market.simulatePrices(m_T);
        sample = payOff(simulations);
    }

    // Estimate the price as the discounted mean of all simulated Payoffs
    double mean = accumulate(payOffs.begin(), payOffs.end(), 0.0) / payOffs.size();
    price = exp(-market.get_r() * m_T) * mean;

    // Estimate the standard error
    double sq_sum = inner_product(payOffs.begin(), payOffs.end(), payOffs.begin(), 0.0);
    double stdev = sqrt(sq_sum / payOffs.size() - mean * mean);
    error = (exp(-market.get_r() * m_T) * stdev)/sqrt(N-1);

    return price;
}

// Print a matrixes from the 3D lattice

void BasketOptionBS::printTimefixed(vector<vector<vector<double>>> Lattice3D, int timeStep) {
    //2D Lattice at fixed time
    std::cout << fixed<<setprecision(2);
    for (int j = Lattice3D[0].size()-1; j >=0; j--) {
        for (int k = Lattice3D.size()-1; k >=0 ; k--) {
            std::cout << right << setw(12) << Lattice3D[k][j][timeStep] << " | ";
        }
        std::cout << endl;
    }
}

void BasketOptionBS::printS1fixed(vector<vector<vector<double>>> Lattice3D, int s1Step) {
    //Top and Botom
    std::cout << fixed << setprecision(2);
    for (int k = 0; k < Lattice3D.size(); k++) {
        for (int i = 0; i < Lattice3D[0][0].size(); i++) {
            std::cout << right << setw(6) << Lattice3D[k][s1Step][i] << " | ";
        }
        std::cout << endl;
    }
}

void BasketOptionBS::printS2fixed(vector<vector<vector<double>>> Lattice3D, int s2Step) {
    //Back and front
    std::cout << fixed << setprecision(2);
    for (int j = Lattice3D[0].size() - 1; j >= 0; j--) {
        for (int i = 0; i < Lattice3D[0][0].size(); i++) {
            std::cout << right << setw(6) << Lattice3D[s2Step][j][i] << " | ";
        }
        std::cout << endl;
    }
}
// Pricing function using Finite Difference Explicit Method
double  BasketOptionBS::priceByEFD(MarketBS market, int timeSteps, int sSteps, double& price) {
    
    //Variable declaration
    double deltaTime, deltaPrice, sMax, sMin = 0;
    deltaTime = m_T / timeSteps;
    sMax = market.get_S0(0) * 3;
    deltaPrice = (sMax - sMin) / sSteps;

    if (market.get_n() == 1) {
        std::cout << "Pricing by EFD with 1 Asset . . ." << endl;

        // Declare matrix for the lattice and a vector for the stock prices
        vector<vector<double>> Lattice2D(sSteps + 1, vector<double>(timeSteps + 1, 0));
        vector <double> sValues(sSteps + 1);


        //Initialize Boundary conditions and a vector with all the prices

        for (int j = 0; j <= sSteps; j++) {
            //Initializa prices vector
            sValues[j] = deltaPrice * j + sMin;
            //Terminal boundary condition
            vector<double> stockPrices;
            stockPrices.push_back(sValues[j]);
            Lattice2D[j][timeSteps] = payOff(stockPrices);
        }
        for (int i = 0; i < timeSteps; i++) {
            //Uper boundary condition
            vector<double> stockPrices;
            stockPrices.push_back(sMax);
            Lattice2D[sSteps][i] = boundaryCond(market, stockPrices, i * deltaTime);
            //lower boundary condition
            stockPrices[0] = sMin;
            Lattice2D[0][i] = boundaryCond(market, stockPrices, i * deltaTime);
        }
        double partial, A, B, C;
        //Populate the rest of the lattice backwards
        for (int i = timeSteps; i > 0; i--) {
            for (int j = 1; j < sSteps; j++) {
                partial = (market.get_sigma(0) * market.get_sigma(0) * sValues[j] * sValues[j]) / deltaPrice;
                A = (deltaTime / (2 * deltaPrice)) * (partial - market.get_r() * sValues[j]);
                B = 1 - market.get_r() * deltaTime - partial * (deltaTime / deltaPrice);
                C = (deltaTime / (2 * deltaPrice)) * (partial + market.get_r() * sValues[j]);
                Lattice2D[j][i - 1] = A * Lattice2D[j - 1][i] + B * Lattice2D[j][i] + C * Lattice2D[j + 1][i];
                if (Lattice2D[j][ i - 1] < 0)
                    Lattice2D[j][ i - 1] = 0;
            }
        }

        //Interpolate values to get the price
        int sInfIndex = (market.get_S0(0) - sMin) / deltaPrice;
        double slope = ((Lattice2D[sInfIndex + 1][0] - Lattice2D[sInfIndex][0]) / (sValues[sInfIndex + 1] - sValues[sInfIndex]));
        price = slope * (market.get_S0(0) - sValues[sInfIndex]) + Lattice2D[sInfIndex][ 0];
    }

    else if (market.get_n() == 2) {
        std::cout << "Pricing by EFD with 2 Asset . . ." << endl;

        // Declare matrix for the lattice and a vector for the stock prices
        vector<vector<vector<double>>> Lattice3D(sSteps + 1, vector<vector<double>>(sSteps + 1, vector<double>(timeSteps + 1, -1)));            
        vector <double> sValues(sSteps + 1);

        for (int s = 0; s <= sSteps; s++) {
            //Initialize prices vector
            sValues[s] = deltaPrice * s + sMin;
        }

        //Boundary conditions
        vector<double> stockPrices;
        stockPrices.push_back(0);
        stockPrices.push_back(0);

        //Terminal boundary condition
        for (int k = 0; k <= sSteps; k++) {
            for (int j = 0; j <= sSteps; j++) {
                stockPrices[0] = sValues[k];
                stockPrices[1] = sValues[j];
                double pay = payOff(stockPrices);
                Lattice3D[k][j][timeSteps] = pay;
            }
        }


        for (int s = 0; s < sSteps; s++) {
            for (int i = 0; i < timeSteps; i++) {

                //Botom boundary condition  k=s ; j=0;
                stockPrices[0] = sValues[s];
                stockPrices[1] = sValues[0];
                Lattice3D[s][0][i] = boundaryCond(market, stockPrices, i * deltaTime);
                //Lattice3D[s][0][i] = 11;

                //Front boundary condition k= sSteps ; j=s;
                stockPrices[0] = sValues[sSteps];
                stockPrices[1] = sValues[s];
                Lattice3D[sSteps][s][i] = boundaryCond(market, stockPrices, i * deltaTime);
                //Lattice3D[sSteps][s][i] = 22;

                //Top boundary condition   k= sSteps - s ; j=sSteps;
               stockPrices[0] = sValues[sSteps - s];
                stockPrices[1] = sValues[sSteps];
                Lattice3D[sSteps - s][sSteps][i] = boundaryCond(market, stockPrices, i * deltaTime);
                //Lattice3D[sSteps - s][sSteps][i] = 33;

                //Back boundary condition  k=0 ; j=sSteps - s;
                stockPrices[0] = sValues[0];
                stockPrices[1] = sValues[sSteps - s];
                Lattice3D[0][sSteps - s][i] = boundaryCond(market, stockPrices, i * deltaTime);
                //Lattice3D[0][sSteps - s][i] = 44;
            }
        }

        //Print the Botom boundary condition matrix
        //std::cout << endl << endl << "Botom boundary condition 2D Lattice" << endl;
       // printS1fixed(Lattice3D, 0);

        //Print the Front boundary condition matrix
        //std::cout << endl << endl << "Front boundary condition 2D Lattice" << endl;
       // printS2fixed(Lattice3D, sSteps);

        //Print the Top boundary condition matrix
        //std::cout << endl << endl << "Top boundary condition 2D Lattice" << endl;
       // printS1fixed(Lattice3D, sSteps);

        //Print the Back boundary condition matrix
        //std::cout << endl << endl << "Back boundary condition 2D Lattice" << endl;
       // printS2fixed(Lattice3D, 0);

        //Print the final boundary condition matrix
        //std::cout << endl << endl << "Final boundary condition 2D Lattice" << endl;
        //printTimefixed(Lattice3D, timeSteps);
        

        double alpha, beta, gamma, delta, epsilon, zeta;
        double A, B, C, D, E, F, G, H, I;
        double sig1 = market.get_sigma(0);
        double sig2 = market.get_sigma(1);
        double r = market.get_r();
        double cor = market.get_cor(0, 1);
        //Compute the lattice backwards
        for (int i = timeSteps; i > 0; i--) {
            for (int j = 1; j < sSteps; j++) {
                for (int k = 1; k < sSteps; k++) {
                    //Calculate 2D PDE coeficients:
                    alpha   = 0.5 * sig1 * sig1 * sValues[j] * sValues[j];
                    beta    = 0.5 * sig2 * sig2 * sValues[k] * sValues[k];
                    gamma   = cor * sig1 * sig2 * sValues[j] * sValues[k];
                    delta   = r * sValues[j];
                    epsilon = r * sValues[k];
                    zeta    = r;

                    //Calculate EFDM Aproximitation coeficients:
                    A = (-gamma)/(4* deltaPrice* deltaPrice);
                    B = ((alpha)/(deltaPrice * deltaPrice)) - ((delta) / (2* deltaPrice));
                    C = (gamma) / (4 * deltaPrice * deltaPrice);
                    D = ((beta) / (deltaPrice * deltaPrice)) + ((epsilon) / (2 * deltaPrice));
                    E = (1/ deltaTime)-((2*alpha+2*beta)/(deltaPrice * deltaPrice))-zeta;
                    F = ((beta) / (deltaPrice * deltaPrice)) - ((epsilon) / (2 * deltaPrice));
                    G = (gamma) / (4 * deltaPrice * deltaPrice);
                    H = ((alpha) / (deltaPrice * deltaPrice)) + ((delta) / (2 * deltaPrice));;
                    I = (-gamma) / (4 * deltaPrice * deltaPrice);
       

                    Lattice3D[k][j][i - 1] =    deltaTime *
                                                (Lattice3D[k+1][j-1][i] * A  + Lattice3D[k][j-1][i] * B  + Lattice3D[k-1][j-1][i] * C    +
                                                 Lattice3D[k+1][j][i] * D    + Lattice3D[k][j][i] * E    + Lattice3D[k-1][j][i] * F      +
                                                 Lattice3D[k+1][j+1][i] * G  + Lattice3D[k][j+1][i] * H  + Lattice3D[k-1][j+1][i] * I)    ;

                    //Lattice3D[k][j][i - 1] = 55 ;

                    if (Lattice3D[k][j][i - 1] < 0)
                        Lattice3D[k][j][i - 1] = 0;
                }
            }
            //Print The 2D Latice at time t.
            //std::cout << endl << "The 2D Latice at time: " << (i-1) * deltaTime << " is:" << endl;
            //printTimefixed(Lattice3D, i-1);
        }

        //Bi-linear Interpolation get the price
        double S1 = market.get_S0(0), S2 = market.get_S0(1);
        int j = (S1 - sMin) / deltaPrice;
        int k = (S2 - sMin) / deltaPrice;
        //std::cout << "S1: " << S1 << endl;
        //std::cout << "S2: " << S2 << endl;
        //std::cout << "Value at j: " << sValues[j] << endl;
        //std::cout << "Value at j+1: " << sValues[j+1] << endl;
        //std::cout << "Precio en k,j: " << Lattice3D[k][j][0] << endl;
        //std::cout << "Precio en k+1,j: " << Lattice3D[k+1][j][0] << endl;
        //std::cout << "Precio en k,j+1: " << Lattice3D[k][j+1][0] << endl;
        //std::cout << "Precio en k+1,j+1: " << Lattice3D[k+1][j + 1][0] << endl << endl;

        alpha = (1)/((sValues[j+1]- sValues[j])*(sValues[k + 1] - sValues[k]));
        beta = Lattice3D[k][j][0]*(sValues[j + 1]-S1)*(sValues[k + 1] - S2);
        gamma = Lattice3D[k][j+1][0] * (S1-sValues[j]) * (sValues[k + 1] - S2);
        delta = Lattice3D[k+1][j][0] * (sValues[j + 1] - S1) * (S2-sValues[k]);
        epsilon = Lattice3D[k+1][j+1][0] * (S1-sValues[j]) * (S2-sValues[k]);
        price = alpha*(beta + gamma + delta + epsilon);

        //price = Lattice3D[k][j][0];
    }

    else {
        std::cout << "Invalid number of Assets for function priceByEFD." << endl;
        price = 0;
    }
    return price;
}

 //destructors
BasketOptionBS::~BasketOptionBS(){}