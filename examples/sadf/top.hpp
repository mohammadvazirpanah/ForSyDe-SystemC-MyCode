#include <forsyde.hpp>

using namespace sc_core;
using namespace ForSyDe;


enum scenarios_state {ADD, MINUS, MUL, DIV};
typedef std::map<scenarios_state,std::tuple<std::array<size_t,1>,std::array<size_t,1>>> scenario_table;


void next_scenario_func_detector(std::array <scenarios_state,2>& next_scenario,
                                const std::array <scenarios_state,2>& current_scenario,
                                const std::vector<int>& inp)
{
    if (current_scenario[0] == ADD && current_scenario[1] == MUL)
    {
        next_scenario[0] = MINUS;
        next_scenario[1] = DIV;

    }
    else
    {
        next_scenario[0] = ADD; 
        next_scenario[1] = MUL; 
    }  
}

void output_decode_func_detector(std::array<scenarios_state,2>& out,
                                const std::array <scenarios_state,2>& current_scenario,
                                const std::vector<int>& inp)
{
    
    if (current_scenario[0] == ADD && current_scenario[1] == MUL)
    {
        out[0] = MINUS;
        out[1] = DIV;

    }
    else
    {
        out[0] = ADD; 
        out[1] = MUL; 
    }    
}

void gamma_func_detector(std::array<size_t,2>& out_rates, const std::array <scenarios_state,2>& scenario)
{
    if (scenario[0] == ADD && scenario[1] == MUL)
    {
        out_rates [0] = 1;
        out_rates [1] = 1;
    }
    else if (scenario[0] == ADD && scenario[1] == DIV)
    {
        out_rates [0] = 1;
        out_rates [1] = 1;
    }
    else if (scenario[0] == MINUS && scenario[1] == MUL)
    {
        out_rates [0] = 1;
        out_rates [1] = 1;
    }
    else if (scenario[0] == MINUS && scenario[1] == DIV)
    {
        out_rates [0] = 1;
        out_rates [1] = 1;
    }
   

}

void func_kernel1(std::vector<int>& out, 
                               const scenarios_state& _scenarios_state, 
                               scenario_table& table,
                               const std::vector<int>& inp)
{
    if (_scenarios_state == ADD)
    {
        out.resize(std::get<1>(table[ADD])[0]);
        out[0] = inp[0] + inp[1] + inp[2];
        std::cout<<"inpADD="<<inp<<std::endl;
        std::cout<<"outADD="<<out<<std::endl;
    }

    else if (_scenarios_state == MINUS)
    {
        out.resize(std::get<1>(table[MINUS])[0]);
        out[0] = inp[1]- inp[0];
        std::cout<<"inpMinus="<<inp<<std::endl;
        std::cout<<"outMinus="<<out<<std::endl;
    }
}


void func_kernel2(std::vector<int>& out, 
                               const scenarios_state& _scenarios_state, 
                               scenario_table& table,
                               const std::vector<int>& inp)
{
    if (_scenarios_state == MUL)
    {
        out.resize(std::get<1>(table[MUL])[0]);
        out[0] = inp[0] * inp[1];
        std::cout<<"inpMUL="<<inp<<std::endl;
        std::cout<<"outMUL="<<out<<std::endl;
    }

    else if (_scenarios_state == DIV)
    {
        out.resize(std::get<1>(table[DIV])[0]);
        out[0] = inp[0] / inp[1];
        std::cout<<"inpDIV="<<inp<<std::endl;
        std::cout<<"outDIV="<<out<<std::endl;
    }
}


SC_MODULE(top)
{
    scenario_table table = 
    {  
        {ADD, std::make_tuple(std::array<size_t,1>{3},std::array<size_t,1>{1})},
        {MINUS, std::make_tuple(std::array<size_t,1>{2},std::array<size_t,1>{1})},
        {MUL, std::make_tuple(std::array<size_t,1>{2},std::array<size_t,1>{1})},
        {DIV, std::make_tuple(std::array<size_t,1>{2},std::array<size_t,1>{1})}
    };

    SADF::signal<scenarios_state> from_detector1, from_detector2 ; 
    SADF::signal<int> from_constant;
    SADF::signal <int> to_kernel1, from_kernel1, to_kernel2, from_kernel2;

 
    SC_CTOR(top)
    {       

        SADF::make_constant ("constant1", 1, 0, from_constant);

        SADF::make_detector12 ("detector12",
                        gamma_func_detector,
                        next_scenario_func_detector,
                        output_decode_func_detector,
                        std::array<scenarios_state,2>{ADD, MUL},
                        1,
                        from_detector1,
                        from_detector2,
                        from_constant
                        );

        SADF::make_kernel21 ("kernel1",
                            func_kernel1,
                            table,
                            from_kernel1,
                            to_kernel1,
                            from_detector1
                            );

        SADF::make_kernel21 ("kernel2",
                            func_kernel2,
                            table,
                            from_kernel2,
                            to_kernel2,
                            from_detector2
                            );

        auto source1 = SADF::make_source ("source1", [] (int& out1, const int& inp1) {out1 = inp1 + 1;}, 1, 20, to_kernel1);
        source1->oport1 (to_kernel2);
        
        SADF::make_sink ("sink1", [] (const int& out) {std::cout <<"kernel1 = " <<out << std::endl;}, from_kernel1);
        SADF::make_sink ("sink2", [] (const int& out) {std::cout <<"kernel2 = " <<out << std::endl;}, from_kernel2);

        // SADF::make_source ("source1", [] (int& out1, const int& inp1) {out1 = inp1 + 1;}, 1, 10, to_kernel1);
        // SADF::make_source ("source2", [] (int& out1, const int& inp1) {out1 = inp1 + 1;}, 1, 10, to_kernel2);



        // auto constant1 = new SADF::constant<int>("constant1", 1, 0);
        // constant1->oport1(form_constant);


        // auto detector12 = new SADF::detector12 <int, scenarios_state, scenarios_state> ("detector12",
        //                                                 gamma_func_detector,
        //                                                 next_state_func_detector,
        //                                                 output_decode_func_detector,
        //                                                 S1);
        // detector12-> iport1 (form_constant);
        // detector12-> oport1 (from_detector1);
        // detector12-> oport2 (from_detector2);



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

