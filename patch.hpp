#ifndef _PATCH_HPP
#define _PATCH_HPP

#include "individual.hpp"

class Patch
{
    public:
        std::vector <Individual> breeders;
        std::vector <Individual> local_offspring;

        Patch(int const nbreeders);

        Patch(Patch const &other);

        void operator=(Patch const &other);

};

#endif
