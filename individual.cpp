#include <random>
#include <iostream>
#include "individual.hpp"
#include "parameters.hpp"


Individual::Individual() :
    is_coop{false}
    ,resources{0.0}
{

}

// birth constructor
Individual::Individual(
        Individual const &parent
        ,std::mt19937 &rng
        ,Parameters const &params) :
    is_coop{parent.is_coop}
    ,resources{0.0}
{
    std::uniform_real_distribution<> uniform{0.0,1.0};

    if (uniform(rng) < params.mu)
    {
        is_coop = !is_coop;
    }
} // end Individual::Individual()

Individual::Individual(Individual const &other) :
    is_coop{other.is_coop}
    ,resources{other.resources}
{}

void Individual::operator=(Individual const &other)
{
    is_coop = other.is_coop;
    resources = other.resources;
}

