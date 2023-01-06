#ifndef _INDIVIDUAL_HPP
#define _INDIVIDUAL_HPP

#include <random>
#include "parameters.hpp"

class Individual
{
    public:
        bool is_coop;

        double resources; // the amount of resources as a result of help

        Individual();

        Individual(Individual const &other);

        Individual(Individual const &other
                ,std::mt19937 &rng
                ,Parameters const &params);

        void operator=(Individual const &other);
};

#endif
