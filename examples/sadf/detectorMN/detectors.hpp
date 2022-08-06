/**********************************************************************
    * detectors.hpp -- Implementation of Detectors                    *
    *                                                                 *
    * Author:                                                         *
    *                                                                 *
    * Purpose: Demonstration of a simple process.                     *
    *                                                                 *
    * Usage:   detector21 and kernel21 tutorial example.              *
    *                                                                 *
    * License:                                                        *
    *******************************************************************/

#ifndef DETECTORS_HPP
#define DETECTORS_HPP

#include <forsyde.hpp>
#include "globals.hpp"

using namespace sc_core;
using namespace ForSyDe;


void gamma_func_detector(std::array<size_t, 2>& out_rates, 
                         const scenarios_state_detector& current_scenario)
{
    if (current_scenario == S1)
    {
        out_rates [0] = 1;
        out_rates [1] = 1;
    }
    else if (current_scenario == S2)
    {
        out_rates [0] = 1;
        out_rates [1] = 1;
    }
    else if (current_scenario == S3)
    {
        out_rates [0] = 1;
        out_rates [1] = 1;
    }
    else if (current_scenario == S4)
    {
        out_rates [0] = 1;
        out_rates [1] = 1;
    }

}

void next_scenario_func_detector (scenarios_state_detector& current_scenario, 
                                    const scenarios_state_detector& previous_scenario, 
                                    const std::tuple <std::vector<int>>& inp)
{
    if (previous_scenario == S1)
    {
        current_scenario = S2;
    }
    else if (previous_scenario == S2)
    {
        current_scenario = S3;
    }
    else if (previous_scenario == S3)
    {
        current_scenario = S4;
    }
    else if (previous_scenario == S4)
    {
        current_scenario = S1;
    }
}


void output_decode_func_detector(std::tuple <scenario_state_kernel1, scenario_state_kernel2>& out,
                                const scenarios_state_detector& current_scenario,
                                const std::tuple <std::vector<int>>& inp)
{
    
    if (current_scenario == S1)
    {
        out = std::make_tuple(ADD, MUL);
    }
    else if (current_scenario == S2)
    {
        out = std::make_tuple(MINUS, DIV);
    }
    else if (current_scenario == S3)
    {
        out = std::make_tuple(ADD, MUL);
    }
    else if (current_scenario == S4)
    {
        out = std::make_tuple(MINUS, DIV);
    }

}

#endif
