#include <forsyde.hpp>

using namespace sc_core;
using namespace ForSyDe;


enum scenarios_state {S1,S2} ;
typedef std::map<scenarios_state,std::tuple<std::array<size_t,1>,std::array<size_t,1>>> scenario_table;


void next_state_func_detector(scenarios_state& next_state,
                                const scenarios_state& current_state,
                                const std::vector<int>& inp)
{
    if (current_state == S1)
    {
        next_state = S2;
    }
    else
    {
        next_state = S1; 
    }    
}

void output_decode_func_detector(std::tuple<scenarios_state, scenarios_state>& out,
                                const scenarios_state& current_state,
                                const std::vector<int>& inp)
{
    if (current_state == S1)
    {
        out = std::make_tuple(S1,S1);
    }
    else
    {
       out = std::make_tuple(S2,S2); 
    }    
}

void gamma_func_detector(std::tuple<size_t, size_t>& out_rates, const scenarios_state& SN)
{
    if (SN == S1)
    {
        out_rates = std::make_tuple(1,2);
    }
    else if (SN == S2)
    {
        out_rates = std::make_tuple(2,3);
    }

}

void func_kernel1(std::vector<int>& out, 
                               const scenarios_state& SS, 
                               scenario_table& table,
                               const std::vector<int>& inp)
{
    std::cout << table << std::endl;
    if (SS == S1)
    {
        out.resize(std::get<1>(table[S1])[0]);
        out[0] = inp[0];
    }

    else if (SS == S2)
    {
        out.resize(std::get<1>(table[S2])[0]);
        out[0] = inp[0];
    }
}


void func_kernel2(std::vector<int>& out, 
                               const scenarios_state& SS, 
                               scenario_table& table,
                               const std::vector<int>& inp)
{
    std::cout << table << std::endl;
    if (SS == S1)
    {
        out.resize(std::get<1>(table[S1])[0]);
        out[0] = inp[0];
    }

    else if (SS == S2)
    {
        out.resize(std::get<1>(table[S2])[0]);
        out[0] = inp[0];
    }
}


SC_MODULE(top)
{
    scenario_table table = 
    {  
        {S1, std::make_tuple(std::array<size_t,1>{1},std::array<size_t,1>{1})},
        {S2, std::make_tuple(std::array<size_t,1>{1},std::array<size_t,1>{1})}
    };

    SADF::signal<scenarios_state> from_detector1, from_detector2 ; 
    SADF::signal<int> form_constant;
    SADF::signal <int> to_kernel1, from_kernel1, to_kernel2, from_kernel2;

 
    SC_CTOR(top)
    {       
        auto constant1 = new SADF::constant<int>("constant1", 1, 0);
        constant1->oport1(form_constant);


        // auto detector12 = new SADF::detector12 <int, scenarios_state, scenarios_state> ("detector12",
        //                                                 gamma_func_detector,
        //                                                 next_state_func_detector,
        //                                                 output_decode_func_detector,
        //                                                 S1);
        // detector12-> iport1 (form_constant);
        // detector12-> oport1 (from_detector1);
        // detector12-> oport2 (from_detector2);

        SADF::make_detector12 ("detector12"
                                ,gamma_func_detector,
                                next_state_func_detector,
                                output_decode_func_detector,
                                S1,
                                from_detector1,
                                from_detector2,
                                form_constant);

        // auto kernel1 = new SADF::kernel <int, scenarios_state, int> ("kernel1",
        //                                                             func_kernel1,
        //                                                             table
        //                                                           );
        // kernel1-> iport1 (to_kernel1);
        // kernel1-> control_port (from_detector1);
        // kernel1-> oport1 (from_kernel1);

        SADF::make_kernel ("kernel1",
                            func_kernel1,
                            table,
                            from_kernel1,
                            to_kernel1,
                            from_detector1
                            );
                            

        auto kernel2 = new SADF::kernel <int, scenarios_state, int> ("kernel2",
                                                                    func_kernel2,
                                                                    table
                                                                  );
        kernel2-> iport1 (to_kernel2);
        kernel2-> control_port (from_detector2);
        kernel2-> oport1 (from_kernel2);


        auto source1 = new SADF::source <int> ("source1", [] (int& out1, const int& inp1) {out1 = inp1 + 1;}, 1, 0);
        source1-> oport1 (to_kernel1);

        auto source2 = new SADF::source <int> ("source2", [] (int& out1, const int& inp1) {out1 = inp1 - 1;}, 1, 0);
        source2-> oport1 (to_kernel2);


        auto sink2 = new SADF::sink <int> ("sink2",[](const int& out) {std::cout << out << std::endl;});
        sink2-> iport1 (from_kernel2);


        auto sink3 = new SADF::sink <int> ("sink3",[](const int& out) {std::cout << out << std::endl;});
        sink3-> iport1 (from_kernel1);


        // auto sink1 = new SADF::sink <scenarios_state> ("sink1",[](const scenarios_state& out) {std::cout << out << std::endl;});
        // sink1-> iport1 (from_detector1);

    }

};

