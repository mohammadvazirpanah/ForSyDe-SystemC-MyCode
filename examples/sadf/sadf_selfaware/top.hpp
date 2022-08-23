/**********************************************************************
    * main.cpp -- the main file and testbench for the SADF tutorial   *
    *                                                                 *
    * Author:  Mohammad Vazirpanah (mohammad.vazirpanah@yahoo.com)    *
    *                                                                 *
    * Purpose: Demonstration of a simple program.                     *
    *                                                                 *
    * Usage:   SADF Tutorial                                          *
    *                                                                 *
    * License: BSD3                                                   *
    *******************************************************************/

#include <forsyde.hpp>

using namespace sc_core;
using namespace ForSyDe;
using namespace std;

enum sim_scenario_type {S1 ,S2};
enum system_scenario_type {sys_resume, sys_stop};
enum inputabst_scenario_type {inputabst_resume, inputabst_stop};
enum self_model_scenario_type {self_model_resume, self_model_stop};
enum outputabst_scenario_type {outputabst_resume, outputabst_stop};
enum env_scenario_type {evn_resume, evn_stop};



// !! ---------------------- Simulator ----------------------  !! // 
typedef map<sim_scenario_type, array<size_t,2>> sim_scenario_table_type;


sim_scenario_table_type sim_table = {
    {S1, {1,1}},
    {S2, {1,1}}
};

// !! ----------------------  System ----------------------  !! // 
typedef std::map<system_scenario_type, std::tuple<size_t,size_t>> system_scenario_table_type;

system_scenario_table_type system_table = 
{  
    {sys_resume, std::make_tuple(1,1)},
    {sys_stop, std::make_tuple(0,0)}
};

// !! ----------------------  Input Abstraction ----------------------  !! // 
typedef std::map<inputabst_scenario_type, std::tuple<size_t,size_t>> inputabst_scenario_table_type;

inputabst_scenario_table_type inputabst_table = 
{  
    {inputabst_resume, std::make_tuple(1,1)},
    {inputabst_stop, std::make_tuple(0,0)}
};

// !! ----------------------  Output Abstraction ----------------------  !! // 
typedef std::map<outputabst_scenario_type, std::tuple<size_t,size_t>> outputabst_scenario_table_type;

outputabst_scenario_table_type outputabst_table = 
{  
    {outputabst_resume, std::make_tuple(1,1)},
    {outputabst_stop, std::make_tuple(0,0)}
};

// !! ---------------------- Self Model ----------------------  !! // 
typedef std::map<self_model_scenario_type, std::tuple<size_t,size_t>> self_model_scenario_table_type;

self_model_scenario_table_type self_model_table = 
{  
    {self_model_resume, std::make_tuple(1,1)},
    {self_model_stop, std::make_tuple(0,0)}
};

// !! ----------------- Abstracted Environment ------------------  !! // 
typedef std::map<env_scenario_type, std::tuple<size_t,size_t>> env_scenario_table_type;

env_scenario_table_type env_table = 
{  
    {evn_resume, std::make_tuple(1,1)},
    {evn_stop, std::make_tuple(0,0)}
};







void enviroment_behavior (std::vector<int>& out, const std::vector<int>& inp)
{

}

void system_behavior(std::vector<int>& out,
                     const system_scenario_type& _system_state,
                     const std::vector<int>&inp)
{

}


void sim_cds_func(sim_scenario_type& new_scenario, 
                        const sim_scenario_type& previous_scenario, 
                        const tuple<vector<int>>& inp)
{

}

void sim_kss_func(tuple<system_scenario_type,
                        inputabst_scenario_type,
                        self_model_scenario_type,
                        outputabst_scenario_type,
                        env_scenario_type>& out,
                        const sim_scenario_type& current_scenario,
                        const tuple<vector<int>>& inp)
{



}

void self_model_func(tuple<vector<int>>& out, 
                    const self_model_scenario_type& _scenarios_state, 
                    const tuple<vector<int>,vector<int>>& inp)
{

}

void inp_abst_func(std::vector<int>& out,
                     const inputabst_scenario_type& _inputabst_state,
                     const std::vector<int>&inp)
{

}


void out_abst_func(std::vector<int>& out,
                     const outputabst_scenario_type& _outputabst_state,
                     const std::vector<int>&inp)
{

}

void self_env_func(std::vector<int>& out,
                     const env_scenario_type& _env_state,
                     const std::vector<int>&inp)
{

}

SC_MODULE(top)
{

    SADF::signal<int> to_enviroment, from_enviroment, to_system, from_system,  to_abstract, from_in_abst, from_out_abst, from_self_model, to_self_model, from_env_abst, from_constant;
    SADF::signal<system_scenario_type> sim_to_system;
    SADF::signal<inputabst_scenario_type> sim_to_in_abst;
    SADF::signal<outputabst_scenario_type> sim_to_out_abst;
    SADF::signal<self_model_scenario_type> sim_to_self_model;
    SADF::signal<env_scenario_type> sim_to_env_abst;




    SC_CTOR(top)
    {
        SDF::make_constant ("constant", 1, 0, from_constant);
        auto environment = SDF::make_comb("enviroment",
                            enviroment_behavior,
                            1,
                            1,
                            from_enviroment,
                            to_enviroment
                            );  
        
        environment->oport1 (to_system);

        auto system = SADF::make_kernel("system",
                            system_behavior,
                            system_table,
                            from_system,
                            sim_to_system,
                            to_system
                        );
        system -> oport1 (to_enviroment);
        

        SADF::make_kernel("input_abstraction",
                            inp_abst_func,
                            inputabst_table,
                            from_in_abst,
                            sim_to_in_abst,
                            from_enviroment
                        );

        SADF::make_kernel("output_abstraction",
                    out_abst_func,
                    outputabst_table,
                    from_out_abst,
                    sim_to_out_abst,
                    from_system
                );

        auto self_model = new SADF::kernelMN<tuple<int>,self_model_scenario_type,tuple<int,int>>(
                        "self_model1",
                        self_model_func,
                        self_model_table
                    );
        self_model->cport1(sim_to_self_model);
        get<0>(self_model->iport)(from_in_abst);
        get<1>(self_model->iport)(from_in_abst);
        get<0>(self_model->oport)(from_self_model);

        SADF::make_kernel("self_env1",
                    self_env_func,
                    env_table,
                    from_env_abst,
                    sim_to_env_abst,
                    from_self_model
                );
        
        auto sim = new SADF::detectorMN<
                                        tuple<system_scenario_type,
                                            inputabst_scenario_type,
                                            self_model_scenario_type,
                                            outputabst_scenario_type,
                                            env_scenario_type>,
                                        tuple<int>,
                                        sim_scenario_type
                                        >
                                    (
                                        "simulator",
                                        sim_cds_func,
                                        sim_kss_func,
                                        sim_table,
                                        S1,
                                        {1}
                                    );
                        
        get<0>(sim->iport)(from_constant);
        get<0>(sim->oport)(sim_to_system);
        get<1>(sim->oport)(sim_to_in_abst);
        get<2>(sim->oport)(sim_to_out_abst);
        get<3>(sim->oport)(sim_to_self_model);
        get<4>(sim->oport)(sim_to_env_abst);
        

    }   


#ifdef FORSYDE_INTROSPECTION
    void start_of_simulation()
    {
        ForSyDe::XMLExport dumper("gen/");
        dumper.traverse(this);
    }
#endif

};

