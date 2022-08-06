/**********************************************************************
    * globals.hpp -- The Globals Definitions                          *
    *                                                                 *
    * Author:                                                         *
    *                                                                 *
    * Purpose: Demonstration of a simple process.                     *
    *                                                                 *
    * Usage:   detector21 and kernel21 tutorial example.              *
    *                                                                 *
    * License:                                                        *
    *******************************************************************/


#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <forsyde.hpp>

using namespace ForSyDe;

enum scenarios_state_detector {S1 ,S2, S3, S4};
enum scenario_state_kernel1 {ADD, MINUS};
enum scenario_state_kernel2 {MUL, DIV};


typedef std::map<scenario_state_kernel1,std::tuple<std::array<size_t,1>,std::array<size_t,1>>> scenario_table_kernel1;
typedef std::map<scenario_state_kernel2,std::tuple<std::array<size_t,1>,std::array<size_t,1>>> scenario_table_kernel2;


scenario_table_kernel1 table_kernel1 = 
{  
    {ADD, std::make_tuple(std::array<size_t,1>{3},std::array<size_t,1>{1})},
    {MINUS, std::make_tuple(std::array<size_t,1>{2},std::array<size_t,1>{1})}
};

scenario_table_kernel2 table_kernel2 =
{
    {MUL, std::make_tuple(std::array<size_t,1>{2},std::array<size_t,1>{1})},
    {DIV, std::make_tuple(std::array<size_t,1>{2},std::array<size_t,1>{1})}
};

#endif
