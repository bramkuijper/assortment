#ifndef _SIMULATION_HPP
#define _SIMULATION_HPP

#include <fstream>
#include <random>
#include "parameters.hpp"

class Simulation
{
    private:
        std::ofstream data_file;
        
        // keep track of the time step of the simulation
        long unsigned time_step = 0;
        
        // random device which is used to generate
        // proper random seeds
        std::random_device rd;

        // store the random seed
        // we need to store this so that we can output the 
        // random seed, so that we could 'replay' the exact
        // same sequence of random numbers for debugging purposes etc
        unsigned int seed;

        // random number generator
        std::mt19937 rng_r;
        
        // uniform distribution to compare against probabilities
        std::uniform_real_distribution<double> uniform;
        
        Parameters params;

    public:
        Simulation(Parameters const &params);
        void write_parameters(std::ofstream &data_file);
};

#endif
