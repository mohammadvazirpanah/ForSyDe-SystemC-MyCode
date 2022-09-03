/**********************************************************************
    * main.cpp --   *
    *                                                                 *
    * Author:  Mohammad Vazirpanah (mohammad.vazirpanah@yahoo.com)    *
    *                                                                 *
    * Purpose: Demonstration of a simple program.                     *
    *                                                                 *
    * Usage:                                             *
    *                                                                 *
    * License: BSD3                                                   *
    *******************************************************************/

#include <forsyde.hpp>


using namespace sc_core;
using namespace ForSyDe;
using namespace std;



enum detector_scenario_type {S1 ,S2, S3, S4};
enum kernel1_scenario_type {ADD, MINUS};
enum kernel2_scenario_type {MUL, DIV};

typedef map<
            detector_scenario_type,
            array<size_t,2>
        > detector_scenario_table_type;

typedef map<
            kernel1_scenario_type,
            tuple<array<size_t,1>,array<size_t,1>>
        > kernel1_scenario_table_type;
typedef map<
            kernel2_scenario_type,
            tuple<array<size_t,1>,array<size_t,1>>
        > kernel2_scenario_table_type;

detector_scenario_table_type detector1_table = {
    {S1, {1,1}},
    {S2, {1,1}},
    {S3, {1,1}},
    {S4, {1,1}}
};

kernel1_scenario_table_type kernel1_table = 
{  
    {ADD,   make_tuple(array<size_t,1>{3},array<size_t,1>{1})},
    {MINUS, make_tuple(array<size_t,1>{2},array<size_t,1>{1})}
};

kernel2_scenario_table_type kernel2_table =
{
    {MUL, make_tuple(array<size_t,1>{2},array<size_t,1>{1})},
    {DIV, make_tuple(array<size_t,1>{2},array<size_t,1>{1})}
};


void kernel1_func(tuple<vector<int>>& out, 
                    const kernel1_scenario_type& _scenarios_state, 
                    const tuple<vector<int>>& inp)
{
    auto inp1 = get<0>(inp);
    auto& out1 = get<0>(out);
    if (_scenarios_state == ADD)
        out1[0] = inp1[0] + inp1[1] + inp1[2];
    else if (_scenarios_state == MINUS)
        out1[0] = inp1[1]- inp1[0];
}


void kernel2_func(tuple<vector<int>>& out, 
                    const kernel2_scenario_type& _scenarios_state, 
                    const tuple<vector<int>>& inp)
{
    auto inp1 = get<0>(inp);
    auto& out1 = get<0>(out);
    if (_scenarios_state == MUL)
        out1[0] = inp1[0] * inp1[1];
    else if (_scenarios_state == DIV)
        out1[0] = inp1[0] / inp1[1];
}

void kernel1_goal (std::vector<bool>& inp)
{

}


void detector1_cds_func(detector_scenario_type& new_scenario, 
                        const detector_scenario_type& previous_scenario, 
                        const tuple<vector<int>>& inp)
{
    switch (previous_scenario)
    {
        case S1:
            new_scenario = S2;
            break;
        case S2:
            new_scenario = S3;
            break;
        case S3:
            new_scenario = S4;
            break;
        case S4:
            new_scenario = S1;
            break;
    }
}


void detector1_kss_func(tuple<kernel1_scenario_type,kernel2_scenario_type>& out,
                        const detector_scenario_type& current_scenario,
                        const tuple<vector<int>>& inp)
{
    switch (current_scenario)
    {
        case S1:
            out = make_tuple(ADD,MUL);
            break;
        case S2:
            out = make_tuple(MINUS,DIV);
            break;
        case S3:
            out = make_tuple(ADD,MUL);
            break;
        case S4:
            out = make_tuple(MINUS,DIV);
            break;
    }
}


SC_MODULE(top)
{
    SADF::signal<kernel1_scenario_type> from_detector1; 
    SADF::signal<kernel2_scenario_type> from_detector2;
    SADF::signal<int> from_source;
    SADF::signal<int> to_kernel1, from_kernel1, to_kernel2, from_kernel2;
    
    SC_CTOR(top)
    {

        auto sourced = new SDF::source<int>("sourced", [] (int& out1, const int& inp1) {out1 = inp1 + 1;}, 1, 4);
        sourced->oport1(from_source);

        auto detector1 = new SADF::simulator<
                                tuple<kernel1_scenario_type,kernel2_scenario_type>,
                                tuple<int>,
                                detector_scenario_type
                            >
                            (
                                "detector1",
                                detector1_cds_func,
                                detector1_kss_func,
                                detector1_table,
                                S1,
                                {1}
                            );
        get<0>(detector1->iport)(from_source);
        get<0>(detector1->oport)(from_detector1);
        get<1>(detector1->oport)(from_detector2);

        auto kernel1 = new SADF::system<tuple<int>,kernel1_scenario_type,tuple<int>>(
                            "kernel1",
                            kernel1_func,
                            kernel1_table,
                            kernel1_goal
                        );
        kernel1->cport1(from_detector1);
        get<0>(kernel1->iport)(to_kernel1);
        get<0>(kernel1->oport)(from_kernel1);
                            
        auto kernel2 = new SADF::kernelMN<tuple<int>,kernel2_scenario_type,tuple<int>>(
                            "kernel2",
                            kernel2_func,
                            kernel2_table
                        );
        kernel2->cport1(from_detector2);
        get<0>(kernel2->iport)(to_kernel2);
        get<0>(kernel2->oport)(from_kernel2);


        auto source1 = new SDF::source<int>("source1", [] (int& out1, const int& inp1) {out1 = inp1 + 1;}, 1, 0);
        source1->oport1(to_kernel1);

        auto source2 = new SDF::source<int>("source2", [] (int& out1, const int& inp1) {out1 = inp1 - 1;}, -1, 0);
        source2->oport1(to_kernel2);


        auto sink1 = new SDF::sink<int>("sink1",[](const int& out) {cout <<"kernel1 = " <<out << endl;});
        sink1->iport1(from_kernel1);


        auto sink2 = new SDF::sink<int>("sink2",[](const int& out) {cout <<"kernel2 = " <<out << endl;;});
        sink2-> iport1(from_kernel2);
    }

};