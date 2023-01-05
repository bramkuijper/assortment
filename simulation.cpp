#include <fstream>
#include "simulation.hpp"
#include "parameters.hpp"

Simulation::Simulation(Parameters const &params_arg) :
    data_file{params_arg.base_name.c_str()} // initialize the data file by giving it a name
    ,rd{}
    ,seed{rd()}
    ,rng_r{seed}
    ,uniform{0.0,1.0}
    ,params{params_arg}
{
}

void Simulation::write_parameters(std::ofstream &data_file)
{
    data_file << "mu;" << params.mu << std::endl;
}
