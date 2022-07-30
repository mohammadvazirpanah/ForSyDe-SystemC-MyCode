#include <forsyde.hpp>


using namespace sc_core;
using namespace ForSyDe;

// void next_state_function(int& NS, const int& CS, const std::tuple<int,int>& inp)
// {


// }

// void output_decoding_function(std::tuple<int,int>& out, const int& CS, const std::tuple<int,int>& inp)

// {


// }

SC_MODULE(top)
{
    // SADF::signal <std::tuple<int,int>> to_detector, from_detector1,from_detector2;
    // SADF::signal <int> from_detector1, from_detector2, to_detector;
   
    SADF::signal <int> to_kernel, from_kernel;
    SADF::signal<ForSyDe::SADF::kernel<int,int>::scenario_type> control_sig ; 

    SC_CTOR(top)
    {       
        // auto detector1 = new SADF::detector2 <int,int,int> ("detector1",
        //                                                     next_state_function,
        //                                                     output_decoding_function,
        //                                                     0);
        //     detector1-> iport1 (to_detector);
        //     detector1-> oport1 (from_detector1);
        //     detector1-> oport2 (from_detector2);
        auto kernel1 = new SADF::kernel <int,int> ("kernel1",
                                                    [](std::vector<int>& out, 
                                                       const ForSyDe::SADF::kernel<int,int>::scenario_type& scenario, 
                                                       const std::vector<int>& inp) 
                                                        {
                                                            std::cout << "kernel" << std::endl;
                                                        }
                                                    );
        kernel1-> iport1 (to_kernel);
        kernel1-> control_port (control_sig);
        kernel1-> oport1 (from_kernel);
    }

};

