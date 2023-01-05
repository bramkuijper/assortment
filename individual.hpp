#ifndef _INDIVIDUAL_HPP
#define _INDIVIDUAL_HPP

#include <random>
#include "parameters.hpp"

class Individual
{
    public:
        bool is_coop;


        Individual();

        Individual(Individual const &other);

        Individual(Individual const &other
                ,std::mt19937 &rng
                ,Parameters const &params);

        void operator=(Individual const &other);
};

#endif
