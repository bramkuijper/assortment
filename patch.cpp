#include "patch.hpp"
#include "individual.hpp"

Patch::Patch(int const nbreeders) :
    breeders(nbreeders)
    ,local_offspring{0}
{}

Patch::Patch(Patch const &other) :
    breeders{other.breeders}
    ,local_offspring{other.local_offspring}
{}

void Patch::operator=(Patch const &other)
{
    breeders = other.breeders;
    local_offspring = other.local_offspring;
}
