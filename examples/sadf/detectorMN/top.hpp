#include <forsyde.hpp>
#include "kernels.hpp"
#include "detectors.hpp"
#include "globals.hpp"

using namespace sc_core;
using namespace ForSyDe;

SC_MODULE(top)
{

    SADF::signal<scenario_state_kernel1> from_detector1; 
    SADF::signal<scenario_state_kernel2> from_detector2;
    SADF::signal<int> from_source;
    SADF::signal <int> to_kernel1, from_kernel1, to_kernel2, from_kernel2;

    SC_CTOR(top)
    {       

        SADF::make_source ("sourced", [] (int& out1, const int& inp1) {out1 = inp1 + 1;}, 1, 4, from_source);

        // SADF::make_detector12 ("detector12",
        //                         gamma_func_detector,
        //                         current_scenario_func_detector,
        //                         output_decode_func_detector,
        //                         S1,
        //                         1,
        //                         from_detector1,
        //                         from_detector2,
        //                         from_source
        //                         );

        SADF::make_kernel21 ("kernel1",
                            func_kernel1,
                            table_kernel1,
                            from_kernel1,
                            to_kernel1,
                            from_detector1
                            );

        SADF::make_kernel21 ("kernel2",
                            func_kernel2,
                            table_kernel2,
                            from_kernel2,
                            to_kernel2,
                            from_detector2
                            );

        auto source1 = SADF::make_source ("source1", [] (int& out1, const int& inp1) {out1 = inp1 + 1;}, 1, 20, to_kernel1);
        source1->oport1 (to_kernel2);
        
        SADF::make_sink ("sink1", [] (const int& out) {std::cout <<"kernel1 = " <<out << std::endl;}, from_kernel1);
        SADF::make_sink ("sink2", [] (const int& out) {std::cout <<"kernel2 = " <<out << std::endl;}, from_kernel2);


        
        
        //! < -------------------------------- Without Using Helper--------------------------------> //!

        auto detectorMN = new SADF::detectorMN <std::tuple<scenario_state_kernel1, scenario_state_kernel2>, std::tuple<int>,scenarios_state_detector>                          ("detectorMN",
                                                           gamma_func_detector,
                                                           next_scenario_func_detector,
                                                           output_decode_func_detector,
                                                           S1,
                                                           1);
        std::get<0>(detectorMN->iport)(from_source);
        std::get<0>(detectorMN->oport)(from_detector1);
        std::get<1>(detectorMN->oport)(from_detector2);



        // auto kernel1 = new SADF::kernel <int, scenarios_state, int> ("kernel1",
        //                                                             func_kernel1,
        //                                                             table
        //                                                           );
        // kernel1-> iport1 (to_kernel1);
        // kernel1-> control_port (from_detector1);
        // kernel1-> oport1 (from_kernel1);

                            
        // auto kernel2 = new SADF::kernel <int, scenarios_state, int> ("kernel2",
        //                                                             func_kernel2,
        //                                                             table
        //                                                           );
        // kernel2-> iport1 (to_kernel2);
        // kernel2-> control_port (from_detector2);
        // kernel2-> oport1 (from_kernel2);


        // auto source1 = new SADF::source <int> ("source1", [] (int& out1, const int& inp1) {out1 = inp1 + 1;}, 1, 0);
        // source1-> oport1 (to_kernel1);

        // auto source2 = new SADF::source <int> ("source2", [] (int& out1, const int& inp1) {out1 = inp1 - 1;}, 1, 0);
        // source2-> oport1 (to_kernel2);


        // auto sink2 = new SADF::sink <int> ("sink2",[](const int& out) {std::cout << out << std::endl;});
        // sink2-> iport1 (from_kernel2);


        // auto sink3 = new SADF::sink <int> ("sink3",[](const int& out) {std::cout << out << std::endl;});
        // sink3-> iport1 (from_kernel1);


        // auto sink1 = new SADF::sink <scenarios_state> ("sink1",[](const scenarios_state& out) {std::cout << out << std::endl;});
        // sink1-> iport1 (from_detector1);

    }

};

