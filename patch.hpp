#ifndef _PATCH_HPP
#define _PATCH_HPP

#include <vector>
#include <set>
#include "individual.hpp"

class Patch
{
    public:
        std::vector <Individual> breeders;
        std::vector <Individual> local_offspring;

        std::vector <int> cooperators;
        std::vector <int> non_cooperators;

        Patch(int const nbreeders);

        Patch(Patch const &other);

        void operator=(Patch const &other);

};

#endif
