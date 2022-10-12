#ifndef ASA_HPP
#define ASA_HPP

#include <forsyde.hpp>


using namespace ForSyDe;
using namespace std;


enum ASAF_scenario_type {normal, violation};
enum sim_scenario_type {start, stop};
enum kernel1_scenario_type {start, stop};
enum kernel2_scenario_type {start, stop};

typedef map<
            ASAF_scenario_type,
            array<size_t,1>
        > detector_scenario_table_type;

ASAF_scenario_table_type ASAF_table = {

    {normal, {1}},
    {violation, {1}}
};




void controller_func ()
{

}

void monitor_func (tuple<vector<bool>>& outs,
                    const tuple<vector<"#sensor">,vector<"#velocity">>& inps)
{
    //Check if the robot is stopped with the velocity sensor

    if (get<1>(inps)[0] == 0)
    {
        outs = make_tuple(vector<bool>{true});
    }
    else
    {
        outs = make_tuple(vector<bool>{false});
    }


}


void ASAF_cds_func(detector_scenario_type& new_scenario, 
                        const detector_scenario_type& previous_scenario, 
                        const tuple<vector<bool>>& inp)
{

        if (inp == true)
        {
            new_scenario = violation;
        }
        else
        {
            new_scenario = normal;
        }
      
}


void ASAF_kss_func(tuple<sim_scenario_type>& out,
                        const detector_scenario_type& current_scenario,
                        const tuple<vector<int>>& inp)
{
    switch (current_scenario)
    {
        case violation:
            out =  make_tuple(start); 
            break;
        case normal:
            out = make_tuple(stop); 
            break;

    }
}

void sim_cds_func()
{

}

void sim_kss_func()
{

}


SC_MODULE(ASA)
{
    SADF::in_port<"#sensor"> iport1, iport2;
    SADF::out_port<"#velocity"> oport1, oport2;
    SADF::signal<int> from_source;



    
    SC_CTOR(ASA)
    {   
        auto sourced = new SDF::source<int>("sourced", [] (int& out1, const int& inp1) {out1 = inp1 + 1;}, 1, 4);
        sourced->oport1(from_source);

        auto controller1 = new SDF::combMN<std::tuple<"#velocity">,              //! Output to the robot 
                                           std::tuple<"#sensor">>                //! Input from the robot 
                                           ("controller1", controller_func, {1}, {1});

        std::get<0>(controller1->iport)(iport1);
        std::get<0>(controller1->oport)(oport1);

        auto monitor1 = new SDF::combMN<std::tuple<bool>,                       //! Output to the ASAF detector for checking violation
                                        std::tuple<"#sensor","#velocity">>      //! Input from the robot for checking when robot stoped
                                        ("controller1", monitor_func, {1}, {1, 1});

        auto ASAF1 = new SADF::detectorMN<
                                    tuple<sim_scenario_type>,                   //! Output to simulator (sim)
                                    tuple<bool>,                                //! Input from monitor               
                                    ASAF_scenario_type                          //! Scenario type (normal, violation)
                                    >
                                (
                                    "ASAF1",
                                    ASAF_cds_func,                              //! current detector scenario function
                                    ASAF_kss_func,                              //! kernel scenario selection function    
                                    ASAF_table,                                 //! Scenario table for the detector
                                    start,                                      //! Initial scenario
                                    {1}                                         //! Number of output tokens
                                );
        get<0>(ASAF1->iport)(from_source);                                      //! Input from source
        get<0>(ASAF1->oport)(from_ASAF1);                                       //! Output to simulator
        get<1>(ASAF1->oport)(from_ASAF2);                                       //! Output to monitor

        auto sim1 = new SADF::kernelMN<
                                    tuple<"#Kernels">,                          //! Output to kernels
                                    tuple<sim_scenario_type>,                   //! Input from ASAF detector             
                                    kernel1_scenario_type                       //! Scenario type (start, stop)
                                    >
                                (
                                    "sim1",
                                    sim_cds_func,                               //! current detector scenario function
                                    sim_kss_func,                               //! kernel scenario selection function    
                                    sim_table,                                  //! Scenario table for the sim detector
                                    start,                                      //! Initial scenario
                                    {1}                                         //! Number of output tokens
                                );

    }
};

#endif