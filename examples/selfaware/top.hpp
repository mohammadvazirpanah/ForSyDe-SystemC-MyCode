

#include <forsyde.hpp>
#include <iostream>
#include <fstream> 

using namespace sc_core;
using namespace ForSyDe;
using namespace std;

enum sim_type {start, stop};
enum sim_scenario_type {S1 ,S2};
enum system_scenario_type {sys_resume, sys_stop};
enum inputabst_scenario_type {inputabst_resume, inputabst_stop};
enum self_model_scenario_type {self_model_resume, self_model_stop};
enum outputabst_scenario_type {outputabst_resume, outputabst_stop};
enum env_scenario_type {evn_resume, evn_stop};



// !! ---------------------- Simulator ----------------------  !! // 
typedef map<sim_scenario_type, array<size_t,5>> sim_scenario_table_type;


sim_scenario_table_type sim_table = {
    {S1, {1,1,1,1,1}},
    {S2, {1,1,1,1,1}}
};

// !! ----------------------  System ----------------------  !! // 
typedef map<system_scenario_type, tuple<array<size_t,1>,array<size_t,2>>> system_scenario_table_type;

system_scenario_table_type system_table = 
{  
    {sys_resume, make_tuple(array<size_t,1>{1},array<size_t,2>{1,1})},
    {sys_stop, make_tuple(array<size_t,1>{0},array<size_t,2>{0,0})}
};

// !! ----------------------  Input Abstraction ----------------------  !! // 
typedef map<inputabst_scenario_type, tuple<size_t,size_t>> inputabst_scenario_table_type;

inputabst_scenario_table_type inputabst_table = 
{  
    {inputabst_resume, make_tuple(1,1)},
    {inputabst_stop, make_tuple(0,0)}
};

// !! ----------------------  Output Abstraction ----------------------  !! // 
typedef map<outputabst_scenario_type, tuple<size_t,size_t>> outputabst_scenario_table_type;

outputabst_scenario_table_type outputabst_table = 
{  
    {outputabst_resume, make_tuple(1,1)},
    {outputabst_stop, make_tuple(0,0)}
};

// !! ---------------------- Self Model ----------------------  !! // 
typedef map<self_model_scenario_type, tuple<array<size_t,2>,array<size_t,1>>> self_model_scenario_table_type;

self_model_scenario_table_type self_model_table = 
{  
    {self_model_resume, make_tuple(array<size_t,2>{1,1},array<size_t,1>{1})},
    {self_model_stop, make_tuple(array<size_t,2>{0,0},array<size_t,1>{0})}
};

// !! ----------------- Abstracted Environment ------------------  !! // 
typedef map<env_scenario_type, tuple<size_t,size_t>> env_scenario_table_type;

env_scenario_table_type env_table = 
{  
    {evn_resume, make_tuple(1,1)},
    {evn_stop, make_tuple(0,0)}
};



void enviroment_behavior (vector<int>& out, const vector<int>& inp)
{   
    
}

void system_goal(vector<bool>& goals)
{

}

void self_model_goal (vector<bool>& goals)
{

}
void system_behavior(tuple<vector<int>, vector<int>>& out,
                     const system_scenario_type& _system_state,
                     const tuple<vector<int>> &inp)
{

}

void sim_cds_func(sim_scenario_type& new_scenario, 
                        const sim_scenario_type& previous_scenario, 
                        const tuple<vector<int>>& inp)
{
        switch (previous_scenario)
    {
        case S1:
            new_scenario = S2;
            break;
        case S2:
            new_scenario = S1;
            break;

    }

}

void sim_kss_func(tuple<system_scenario_type,
                        inputabst_scenario_type,
                        self_model_scenario_type,
                        outputabst_scenario_type,
                        env_scenario_type>& out,
                        const sim_scenario_type& current_scenario,
                        const tuple<vector<int>>& inp)
{

    switch (current_scenario)
    {
        case S1:
            out = make_tuple(sys_resume,
                            inputabst_resume, 
                            self_model_resume,
                            outputabst_resume,
                            evn_resume);
            break;
        case S2:
            out = make_tuple(sys_stop,
                            inputabst_stop,
                            self_model_stop,
                            outputabst_stop,
                            evn_stop);
            break;
    }

}

void self_model_func(tuple<vector<int>>& out, 
                    const self_model_scenario_type& _scenarios_state, 
                    const tuple<vector<int>,vector<int>>& inp)
{

}

void inp_abst_func(vector<int>& out,
                     const inputabst_scenario_type& _inputabst_state,
                     const vector<int>&inp)
{

}


void out_abst_func(vector<int>& out,
                     const outputabst_scenario_type& _outputabst_state,
                     const vector<int>&inp)
{

}

void self_env_func(vector<int>& out,
                     const env_scenario_type& _env_state,
                     const vector<int>&inp)
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


        auto environment1 = new SADF::environment<int,int> ("enviroment",enviroment_behavior, 1, 1);
        environment1->iport1 (to_enviroment);
        environment1->oport1 (from_enviroment);
        environment1->oport1 (to_system);

        auto system1 = new SADF::SA<
                                        tuple<int,int>,
                                        system_scenario_type,
                                        tuple<int>
                                        >
                                        (
                                        "system1",
                                        system_behavior,
                                        system_table,
                                        system_goal
                                        );
        system1->cport1(sim_to_system);
        get<0>(system1->iport)(to_system);
        get<0>(system1->oport)(to_enviroment);
        get<1>(system1->oport)(from_system);
                                               

        auto inputabst1 = new SADF::kernel<int,inputabst_scenario_type, int>
                                                    (
                                                    "inputabst",
                                                    inp_abst_func,
                                                    inputabst_table
                                                    );
        inputabst1->cport1(sim_to_in_abst);
        inputabst1->iport1(from_enviroment);
        inputabst1->oport1(from_in_abst);
 

        auto outputabst1 = new SADF::kernel<int,outputabst_scenario_type, int>
                                                    (
                                                    "outputabst",
                                                    out_abst_func,
                                                    outputabst_table
                                                    );
        outputabst1->cport1(sim_to_out_abst);
        outputabst1->iport1(from_system);
        outputabst1->oport1(from_out_abst);
        

        auto selfmodel = new SADF::SA<
                                        tuple<int>,
                                        self_model_scenario_type,
                                        tuple<int,int>
                                        >
                                        (
                                        "selfmodel1",
                                        self_model_func,
                                        self_model_table,
                                        self_model_goal
                                        );
        selfmodel->cport1(sim_to_self_model);
        get<0>(selfmodel->iport)(from_in_abst);
        get<1>(selfmodel->iport)(from_env_abst);
        get<0>(selfmodel->oport)(from_self_model);


        SADF::make_kernel("self_env1",
                    self_env_func,
                    env_table,
                    from_env_abst,
                    sim_to_env_abst,
                    from_self_model
                );
        
        auto sim = new SADF::simulator<
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
        get<2>(sim->oport)(sim_to_self_model);
        get<3>(sim->oport)(sim_to_out_abst);
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

