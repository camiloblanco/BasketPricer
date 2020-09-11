/****************************************************************************************
*								#INCLUDES AND #CONSTANTS								*
****************************************************************************************/
#include "MarketBS.h"

/****************************************************************************************
*									MEMBER FUNCTIONS									*
****************************************************************************************/


//Default constructor
MarketBS::MarketBS() : m_n(0), m_S0(0), m_sigma(0), m_r(0) {
}


//Parametrized constructor
MarketBS::MarketBS(int n, vector<double> S0, vector<double> sigma, double r, MatrixXd CorM) : m_n(n), m_S0(S0), m_sigma(sigma), m_r(r), m_CorM(CorM){
    m_ChoL = m_CorM.llt().matrixL();
}

//accessors
vector<double> MarketBS::get_S0() { return m_S0; }
vector<double> MarketBS::get_sigma() { return m_sigma; }
double MarketBS::get_sigma (int n) { return m_sigma[n]; }
double MarketBS::get_S0 (int n) { return m_S0[n]; }
double MarketBS::get_r() { return m_r; }
double MarketBS::get_n() { return m_n; }
double MarketBS::get_cor(int n, int m) { return m_CorM(n,m); }

//Simulate an Stock Prices future values for path independent options
vector<double>  MarketBS::simulatePrices(double T) {

    if (T > 0) {
        //Initialize variables
        vector<double> simulations(m_n);
 
        //Mersenne twister MT19937 random and normal distribution object with mean 0 and dev 1

        auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
        mt19937 generator(seed);
        normal_distribution<double> distribution(0.0, 1.0);

        auto gen = [&distribution, &generator]() {
            return distribution(generator);
        };

        //Generate a vector of Independent Normal Distibuted Randon Variables
        vector<double> Zk(m_n);
        generate(begin(Zk), end(Zk), gen);

        //Typecast from Zk std::Vector to an eignen vector Vx 
        VectorXd Vx = Map<VectorXd, Unaligned>(Zk.data(), Zk.size());

        //Correlate the Normal Distibuted Randon Variables using the Cholesky factor L
        Vx = m_ChoL * Vx;

        //Typecast from the eigen vector Vx to the ZkCor std::Vector
        vector<double> ZkCor(Vx.data(), Vx.data() + Vx.size());

        // Simulate for the n assets
        for (int i = 0;i< m_n;++i)
        {
            simulations[i] = m_S0[i] * exp((m_r - m_sigma[i] * m_sigma[i] * 0.5) * T + m_sigma[i] * sqrt(T) * ZkCor[i]);
        }
        return simulations;
    }
    cout << "Error in MarketBS::simulatePrices, Partameter T should be positive and greater than 0." << endl;
    exit(0);
}

//Default destructor
MarketBS::~MarketBS() {}