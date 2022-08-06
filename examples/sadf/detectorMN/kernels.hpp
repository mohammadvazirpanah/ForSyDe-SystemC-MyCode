/**********************************************************************
    * kernels.hpp -- Implementation of Kernels                        *
    *                                                                 *
    * Author:                                                         *
    *                                                                 *
    * Purpose: Demonstration of a simple process.                     *
    *                                                                 *
    * Usage:   detector21 and kernel21 tutorial example.              *
    *                                                                 *
    * License:                                                        *
    *******************************************************************/

#ifndef KERNELS_HPP
#define KERNELS_HPP

#include <forsyde.hpp>
#include "globals.hpp"

using namespace sc_core;
using namespace ForSyDe;


void func_kernel1(std::vector<int>& out, 
                               const scenario_state_kernel1& _scenarios_state, 
                               scenario_table_kernel1& table,
                               const std::vector<int>& inp)
{
    if (_scenarios_state == ADD)
    {
        out.resize(std::get<1>(table[ADD])[0]);
        out[0] = inp[0] + inp[1] + inp[2];
        // std::cout<<"inp ADD K1="<<inp<<std::endl;
        // std::cout<<"out ADD K1="<<out<<std::endl;
    }

    else if (_scenarios_state == MINUS)
    {
        out.resize(std::get<1>(table[MINUS])[0]);
        out[0] = inp[1]- inp[0];
        // std::cout<<"inp Minus K1="<<inp<<std::endl;
        // std::cout<<"out Minus K1="<<out<<std::endl;
    }
}


void func_kernel2(std::vector<int>& out, 
                               const scenario_state_kernel2& _scenarios_state, 
                               scenario_table_kernel2& table,
                               const std::vector<int>& inp)
{
    if (_scenarios_state == MUL)
    {
        out.resize(std::get<1>(table[MUL])[0]);
        out[0] = inp[0] * inp[1];
        // std::cout<<"inp MUL K2="<<inp<<std::endl;
        // std::cout<<"out MUL K2="<<out<<std::endl;
    }

    else if (_scenarios_state == DIV)
    {
        out.resize(std::get<1>(table[DIV])[0]);
        out[0] = inp[0] / inp[1];
        // std::cout<<"inp DIV K2="<<inp<<std::endl;
        // std::cout<<"out DIV K2="<<out<<std::endl;
    }
}


#endif
