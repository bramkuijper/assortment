#include <fstream>
#include <cassert>
#include <algorithm>
#include <vector>
#include <array>
#include "simulation.hpp"
#include "parameters.hpp"
#include "patch.hpp"

Simulation::Simulation(Parameters const &params_arg) :
    data_file{params_arg.base_name.c_str()} // initialize the data file by giving it a name
    ,rd{}
    ,seed{rd()}
    ,rng_r{seed}
    ,uniform{0.0,1.0}
    ,patch_sampler(0,params.npatches-1)
    ,params{params_arg}
    ,metapopulation(params_arg.npatches, Patch(params_arg.npp))
    ,payoff_matrix{{1.0, 1.0 + params_arg.B},{1.0 - params.C,1.0 - params.C + params.B + params.D}}
{
    // number of breeders have to be even 
    // coz pairwise interactions
    assert(params_arg.npp % 2 == 0);
} // end Simulation()

void Simulation::run()
{
    write_data_headers();
    for (generation = 0; generation < params.max_time; ++generation)
    {
        reset_resources();

        help_pairwise();

        mortality_replacement();

        if (generation % params.output_nth_generation == 0)
        {
            write_data();
        }
    }

    write_parameters();
} // end Simulation::run()

void Simulation::write_parameters()
{
    data_file << std::endl << std::endl
                << "seed;" << seed << std::endl
                << "mu;" << params.mu << std::endl
                << "max_time;" << params.max_time << std::endl
                << "npp;" << params.npp << std::endl
                << "B;" << params.B << std::endl
                << "C;" << params.C << std::endl
                << "D;" << params.D << std::endl
                << "alpha;" << params.alpha << std::endl
                << "mortality_prob;" << params.mortality_prob << std::endl
                << "n_parents_sample;" << params.n_parents_sample << std::endl;

} // end Simulation::write_parameters()


void Simulation::pairwise_interact(int const patch_idx, int const ind1, int const ind2)
{
    assert(ind1 >= 0);
    assert(ind1 < params.npp);

    assert(ind2 >= 0);
    assert(ind2 < params.npp);

    bool ind1_cooperates = 
        metapopulation[patch_idx].breeders[ind1].is_coop;
    bool ind2_cooperates = 
        metapopulation[patch_idx].breeders[ind2].is_coop;

    metapopulation[patch_idx].breeders[ind1].resources = 
        payoff_matrix[ind1_cooperates][ind2_cooperates];

    metapopulation[patch_idx].breeders[ind2].resources = 
        payoff_matrix[ind2_cooperates][ind1_cooperates];
}

void Simulation::help_pairwise()
{
    double Ef = 1.0;

    int ind1, ind2;

    int n_coop_assortative, n_non_coop_assortative;

    // max number of interactions anyone could have would be n
    // so there may be a lot of individuals with 0 interactions which do not perform well
    // however, this is all driven by change so should not affect selection for 
    // p
    for (int patch_idx  = 0; patch_idx < params.npatches; ++patch_idx)
    {
        // calculate number of assortative cooperators
        std::binomial_distribution<int> 
            assortative_coop_sampler(
                    (double)(metapopulation[patch_idx].cooperators.size())/2.0, 
                    params.alpha);

        // sample only even numbers
        n_coop_assortative = 2*assortative_coop_sampler(rng_r);

        assert(n_coop_assortative >= 0);
        assert(n_coop_assortative <= metapopulation[patch_idx].cooperators.size());
        

        // calculate number of assortative non-cooperators
        std::binomial_distribution<int> 
            assortative_non_coop_sampler(
                                (double)(metapopulation[patch_idx].non_cooperators.size())/2.0,                    params.alpha);

        // sample only even numbers
        n_non_coop_assortative = 2*assortative_non_coop_sampler(rng_r);

        assert(n_non_coop_assortative >= 0);
        assert(n_non_coop_assortative <= metapopulation[patch_idx].non_cooperators.size());

        // shuffle cooperators
        std::shuffle(metapopulation[patch_idx].cooperators.begin(),
                metapopulation[patch_idx].cooperators.end(),
                rng_r);

        // shuffle non-cooperators
        std::shuffle(metapopulation[patch_idx].non_cooperators.begin(),
                metapopulation[patch_idx].non_cooperators.end(),
                rng_r);

        for (int i = 0; i < n_coop_assortative; i+=2)
        {
            ind1 = metapopulation[patch_idx].cooperators[i];
            ind2 = metapopulation[patch_idx].cooperators[i+1];

            pairwise_interact(patch_idx, ind1, ind2);
        }
        
        for (int i = 0; i < n_non_coop_assortative; i+=2)
        {
            ind1 = metapopulation[patch_idx].non_cooperators[i];
            ind2 = metapopulation[patch_idx].non_cooperators[i+1];

            pairwise_interact(patch_idx, ind1, ind2);
        }

        // copy the remainder over in a general array
        std::vector <int> non_assortative(
                metapopulation[patch_idx].cooperators.begin() + n_coop_assortative
                ,metapopulation[patch_idx].cooperators.end());

        for (int i = n_non_coop_assortative; 
                i < metapopulation[patch_idx].non_cooperators.size(); ++i)
        {
            non_assortative.push_back(
                    metapopulation[patch_idx].non_cooperators[i]
                    );
        }

        // random shuffle the non_assortative vector
        std::shuffle(
                non_assortative.begin()
                ,non_assortative.end()
                ,rng_r);

        for (int i = 0; i < non_assortative.size(); i+=2)
        {
            ind1 = non_assortative[i];
            ind2 = non_assortative[i+1];

            pairwise_interact(patch_idx, ind1, ind2);
        }
    } // end int patch_idx
} // end Simulation::help_pairwise()


void Simulation::reset_resources()
{
    for (int patch_idx = 0; 
            patch_idx < params.npatches; ++patch_idx)
    {
        for (int individual_idx = 0;
                individual_idx < params.npp; ++individual_idx)
        {
            metapopulation[patch_idx].breeders[individual_idx].resources = 0.0;
        }
    }
} // end Simulation::reset_resources()

// have individuals die and then replace them within a patch
void Simulation::mortality_replacement()
{
    std::vector <int> parents(params.n_parents_sample,-1);
    std::vector <int> patch_parents(params.n_parents_sample, -1);
    std::vector <double> fecundity(params.n_parents_sample, 0.0);

    int patch_of_origin,parent;

    std::uniform_int_distribution<int> 
        parent_sampler(0,params.npp - 1);

    for (int patch_idx = 0; 
            patch_idx < params.npatches; ++patch_idx)
    {
        for (int individual_idx = 0;
                individual_idx < params.npp; ++individual_idx)
        {
            if (uniform(rng_r) < params.mortality_prob) // individual survives, move on
            {
                for (int sample_idx = 0; 
                        sample_idx <  params.n_parents_sample; ++sample_idx)
                {
                    patch_of_origin = patch_idx;

                    if (uniform(rng_r) < params.d)
                    {
                        patch_of_origin = patch_sampler(rng_r);
                    }

                    parent = parent_sampler(rng_r);

                    parents[sample_idx] = parent;
                    patch_parents[sample_idx] = patch_of_origin;

                    fecundity[sample_idx] = 
                        metapopulation[patch_of_origin].breeders[parent].resources;
                }

                // make within-patch distribution of fecundities
                std::discrete_distribution <int> fecundity_sampler(
                        fecundity.begin()
                        ,fecundity.end()
                        );

                int sampled_fecundity_idx = fecundity_sampler(rng_r);

                int sampled_parent = parents[sampled_fecundity_idx];

                int sampled_patch = patch_parents[sampled_fecundity_idx];

                assert(sampled_patch >= 0);
                assert(sampled_patch < params.npatches);

                assert(sampled_parent >= 0);
                assert(sampled_parent < params.npp);


                Individual Kid(metapopulation[sampled_patch].breeders[sampled_parent]
                        ,rng_r
                        ,params);

                metapopulation[patch_idx].breeders[individual_idx] = Kid;
            }
        } // end for individual_idx
    } // end for int patch_idx // end for()
} // end Simulation::mortality_replacement()

void Simulation::write_data()
{
    double mean_coop = 0.0;
    double ss_coop = 0.0;

    double p;

    for (int patch_idx = 0; 
            patch_idx < params.npatches; ++patch_idx)
    {
        for (int individual_idx = 0; 
                individual_idx < params.npp; ++individual_idx)
        {
             p = metapopulation[patch_idx].breeders[individual_idx].is_coop;

             mean_coop += p;
             ss_coop += p * p;
        }
    }

    mean_coop /= params.npatches * params.npp;

    double var_coop = ss_coop / (params.npatches * params.npp) 
        - mean_coop * mean_coop;

    data_file << generation << ";" << 
        mean_coop << ";" << 
        var_coop << ";" << std::endl;
} // end write_data_headers

void Simulation::write_data_headers()
{
    data_file << "generation;mean_coop;var_coop;" << std::endl;
}
