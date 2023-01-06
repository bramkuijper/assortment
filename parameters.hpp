#ifndef _PARAMETERS_HPP
#define _PARAMETERS_HPP

#include <string>

class Parameters
{
    public:
        double mu = 0.001;
        std::string base_name = "output_gardner_2011";
        unsigned npatches = 1;
        int max_time = 50000;
        int npp = 5000;
        double B = 3.0;
        double C = 1.0;
        double D = 2.0;
        double alpha = 0.5;
        double d = 0.0;
        double mortality_prob = 0.1;
        int n_parents_sample = 20;
        int output_nth_generation = 10;
};

#endif
