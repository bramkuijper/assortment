#include <vector>
#include <string>
#include "parameters.hpp"
#include "simulation.hpp"

// the brunt of the code
int main(int argc, char **argv)
{
    Parameters params;

    params.B = std::stod(argv[1]);
    params.C = std::stod(argv[2]);
    params.D = std::stod(argv[3]);
    params.base_name = argv[4];

    Simulation sim(params);

    sim.run();

}
