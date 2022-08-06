/**********************************************************************           
    * sadf_process_constructors.hpp -- Process constructors in the    *
    *                                scenario aware data flow MOC.    *
    *                                                                 *
    * Author:                                                         *
    *                                                                 *
    * Purpose: Providing basic process constructors for modeling      *
    *          SADF systems in ForSyDe-SystemC                        *
    *                                                                 *
    * Usage:   This file is included automatically                    *
    *                                                                 *
    * License:                                                        *
    *******************************************************************/

#ifndef SADF_PROCESS_CONSTRUCTORS_HPP
#define SADF_PROCESS_CONSTRUCTORS_HPP

/*! \file sadf_process_constructors.hpp
 * \brief Implements the basic process constructors in the SADF MoC
 * 
 *  This file includes the basic process constructors used for modeling
 * in the SADF model of computation.
 */

#include <functional>
#include <tuple>
#include <vector>
#include <map>
#include "sadf_process.hpp"

namespace ForSyDe
{

namespace SADF
{

using namespace sc_core;

//! Process constructor for a constant source process
/*! This class is used to build a souce process with constant output.
 * Its main purpose is to be used in test-benches.
 * 
 * This class can directly be instantiated to build a process.
 */
template <class T>
class constant : public sadf_process
{
public:
    SADF_out<T> oport1;            ///< port for the output channel

    //! The constructor requires the module name
    /*! It creates an SC_THREAD which runs the user-imlpemented function
     * and writes the result using the output port
     */
    constant(sc_module_name _name,               ///< The module name
              T init_val,                       ///< The constant output value
              unsigned long long take=0        ///< number of tokens produced (0 for infinite)
             ) : sadf_process(_name), oport1("oport1"),
                 init_val(init_val), take(take)
                 
    {
#ifdef FORSYDE_INTROSPECTION
        std::stringstream ss;
        ss << init_val;
        arg_vec.push_back(std::make_tuple("init_val", ss.str()));
        arg_vec.push_back(std::make_tuple("take", std::to_string(take)));
#endif
    }
    
    //! Specifying from which process constructor is the module built
    std::string forsyde_kind() const {return "SADF::constant";}
    
private:

    // The current state
    T init_val;

    // Number of tokens produced
    unsigned long long take;    
    
    unsigned long long tok_cnt;
    bool infinite;
    
    //Implementing the abstract semantics
    void init()
    {
        if (take==0) infinite = true;
        tok_cnt = 0;
    }
    
    void prep() {}
    
    void exec() {}
    
    void prod()
    {
        if (tok_cnt++ < take || infinite)
            WRITE_MULTIPORT(oport1, init_val)
        else wait();
    }
    
    void clean() {}

#ifdef FORSYDE_INTROSPECTION
    void bindInfo()
    {
        boundOutChans.resize(1);    // only one output port
        boundOutChans[0].port = &oport1;
    }
#endif
};

//! Process constructor for a Detector process
/*! This class is used to build Detector.
 * Given an initial scenario, a gamma function for output rate, and an output decoding
 * function it creates a Detector process.
 */
template <class TI, class TS, class TO1, class TO2>
class detector12 : public sadf_process
{
public:

    SADF_in<TI>  iport1;   // input port
    SADF_out<TO1> oport1;   // output port
    SADF_out<TO2> oport2;   // output port
    
    //! Type of the decoding output rate function for each output port to be passed to the process constructor
    typedef std::function<void(std::array<size_t,2>&, 
                                const TS&)> gamma_functype;

    //! Type of the current scenario function to be passed to the process constructor
    typedef std::function<void(TS&,
                              const TS&,
                              const std::vector<TI>&)> cs_functype;
                              
    //! Type of the output-decoding function to be passed to the process constructor
    typedef std::function<void(std::tuple<TO1,TO2>&,
                                const TS&,
                                const std::vector<TI>&)> od_functype;
    
    //! The constructor requires the module name
    /*! It creates an SC_THREAD which reads data from its input port,
     * applies the user-imlpemented functions to the input and current
     * state and writes the results using the output port
     */
    detector12(const sc_module_name& _name,         ///< The module name
            const gamma_functype& _gamma_func,      ///< The output rate function for each output port
            const cs_functype& _ns_func,            ///< The next scenario function
            const od_functype& _od_func,            ///< The output-decoding function
            const TS& init_sc,                      ///< Initial scenario
            const unsigned int& itoks               ///< consumption rate for the first input
            ) : sadf_process(_name), _gamma_func(_gamma_func), _ns_func(_ns_func),
              _od_func(_od_func), init_sc(init_sc), itoks(itoks)
    {
#ifdef FORSYDE_INTROSPECTION
        std::string func_name = std::string(basename());
        func_name = func_name.substr(0, func_name.find_last_not_of("0123456789")+1);
        arg_vec.push_back(std::make_tuple("_gamma_func",func_name+std::string("_gamma_func")));
        arg_vec.push_back(std::make_tuple("_ns_func",func_name+std::string("_ns_func")));
        arg_vec.push_back(std::make_tuple("_od_func",func_name+std::string("_od_func")));
        std::stringstream ss;
        ss << init_sc;
        arg_vec.push_back(std::make_tuple("init_sc",ss.str()));
        arg_vec.push_back(std::make_tuple("itoks",std::to_string(itoks)));
#endif
    }
    
    //! Specifying from which process constructor is the module built
    std::string forsyde_kind() const{return "SADF::detector12";}
    
private:
    //! The functions passed to the process constructor
    gamma_functype _gamma_func;
    cs_functype _ns_func;
    od_functype _od_func;
    
    // Input, output, current scenario, previous scenario and input tokens variables
    std::array<size_t,2> out_rates;
    std::tuple<TO1,TO2> ovals;
    std::vector<TI> ivals;
    TS* csvals; 
    TS* psvals; 
    TS init_sc;
    unsigned int itoks;

    //Implementing the abstract semantics
    void init()
    {
        // Initialize the initial scenario and the resize the input vector
        ivals.resize(itoks);
        
        csvals = new TS;
        psvals = new TS;
        *csvals = init_sc;
    }
    
    void prep()
    {
        //! Reading the input port according to the input tokens consumption rate which is passed to the constructor
        for (auto it=ivals.begin();it!=ivals.end();it++)
            *it = iport1.read();
    }

    void exec()
    {

        //! Applying the output rate function to the current scenario to get the output rates for each output port
        _gamma_func(out_rates, *csvals);

        /*! Applying the output-decoding function to the current scenario and the input tokens.
        *   The output-decoding function is user-implemented function which is passed to the constructor 
        *   to determine scenario for each output port (control token for sending to the kernel)
        */
        _od_func(ovals, *csvals, ivals);

        //! Update the previous scenario for next round
        *psvals = *csvals;

        //! Applying the next scenario function to to the previous scenario and input tokens to get the next scenario for next round
        _ns_func (*csvals, *psvals, ivals);
    }
    
    void prod()
    {   
        //! Writing the output port according to the output token production rates which is detemined by the gamma function in execute stage

        //! Writing the output port 1 (associated with one control token)
        for (auto it=0; it<out_rates[0]; it++)
            WRITE_MULTIPORT(oport1, std::get<0>(ovals))

        //! Writing the output port 2 (associated with one control token)
        for (auto it=0; it<out_rates[1]; it++)
            WRITE_MULTIPORT(oport2, std::get<1>(ovals))

    }
    
    void clean()
    {
        delete csvals;        
        delete psvals;
    }

#ifdef FORSYDE_INTROSPECTION
    void bindInfo()
    {
        boundInChans.resize(1);     // only one input port
        boundInChans[0].port = &iport1;
        boundOutChans.resize(1);    // only one output port
        boundOutChans[0].port = &oport1;
        boundOutChans.resize(1);    // only one output port
        boundOutChans[0].port = &oport2;
    }
#endif
};

//! Process constructor for a kernel process with two inputs and one output
/*! This class is used to build kernel processes with two inputs (control and data) and one output.
 * The class is parameterized for inputs, output and control signals
 * data-types.
 */

template <class TI, class TCT, class TO>

class kernel21 : public sadf_process
{
public:
 
    SADF_in<TI> iport1;            // input port
    SADF_in <TCT> control_port;    // control port which is connected to the detector
    SADF_out<TO> oport1;           // output port

    //! Type of the table of kernel's scennarios to be passed to the process constructor
    typedef std::map<TCT,std::tuple<std::array<size_t,1>,std::array<size_t,1>>> scenario_table;

    //! Type of the kernel function to be passed to the process constructor
    typedef std::function<void(
                               std::vector<TO>&, 
                               const TCT&, 
                               scenario_table&,
                               const std::vector<TI>&
                               )> kernel_functype;

    //! The constructor requires the module name
    /*! It creates an SC_THREAD which reads data from its input port,
     * applies the user-imlpemented functions to the input and current
     * scenario and writes the results using the output port
     */      

    kernel21(const sc_module_name& _name,                //<The Module name 
           const kernel_functype& _func,                 //<The kernel function
           const scenario_table& _scenario_table         //<The table of kernel's scenarios
          ) : sadf_process(_name), iport1("iport1"), control_port("control_port"), oport1("oport1"),
           _func(_func), _scenario_table (_scenario_table)
    {
#ifdef FORSYDE_INTROSPECTION
        std::string func_name = std::string(basename());
        func_name = func_name.substr(0, func_name.find_last_not_of("0123456789")+1);
        arg_vec.push_back(std::make_tuple("_func",func_name+std::string("_func")));
        arg_vec.push_back(std::make_tuple("_scenario_table",std::to_string(_scenario_table)));
#endif
    }
    
    //! Specifying from which process constructor is the module built
    std::string forsyde_kind() const{return "SADF::kernel21";}
    
private:
    
    // Inputs, output and control port variables
    std::vector<TO> o1vals;
    std::vector<TI> i1vals;
    TCT cntl_port; 


    //! The functions passed to the process constructor
    kernel_functype _func;

    //! The table of kernel's scenarios to be passed to the process constructor
    scenario_table _scenario_table;

    //! Consumption, production rates and input-output tokens variables
    size_t o1toks, i1toks;
    size_t cons_rate;
    size_t prod_rate;

    //Implementing the abstract semantics
    void init()
    {

    }
    
    void prep()
    {
        //! Reading the control port which is connected to the detector to determine the scenario for the kernel
        cntl_port = control_port.read();
        
        //! Set the consumption and production rates from the kernel's scenario table (table passed to the process constructor)
        //! first element of the tuple is use for the consumption rate, second element is the production rate
        cons_rate = std::get<0>(_scenario_table[cntl_port])[0];
        prod_rate = std::get<1>(_scenario_table[cntl_port])[0];

        //! Resizing the input and output vectors according to the consumption and production rates
        i1vals.resize(cons_rate);
        o1vals.resize(prod_rate);
        
        //! Reading the input port (iport1) according to the consumption rate (cons_rate)
        for (auto it=i1vals.begin();it!=i1vals.end();it++)
            *it = iport1.read();
        
    }
    
    void exec()
    {   
        //! Calling the user-imlpemented kernel function with input, output vectors, control port and scenario table
        _func(o1vals, cntl_port, _scenario_table, i1vals); 
    }
    
    void prod()
    {
        //! Writing the output values into output port which is set in user-imlpemented kernel function
        WRITE_VEC_MULTIPORT(oport1, o1vals)
        o1vals.clear();
    }
    
    void clean()
    {

    }
#ifdef FORSYDE_INTROSPECTION
    void bindInfo()
    {
        boundInChans.resize(1);     // only one input port
        boundInChans[0].port = &iport1;
         boundInChans.resize(1);     // only one control port
        boundInChans[0].port = &control_port;
        boundOutChans.resize(1);    // only one output port
        boundOutChans[0].port = &oport1;
    }
#endif
};


template <class T>
class sink : public sadf_process
{
public:
    SADF_in<T> iport1;         ///< port for the input channel
    
    //! Type of the function to be passed to the process constructor
    typedef std::function<void(const T&)> functype;

    //! The constructor requires the module name
    /*! It creates an SC_THREAD which runs the user-imlpemented function
     * in each cycle.
     */
    sink(sc_module_name _name,      ///< process name
         functype _func             ///< function to be passed
        ) : sadf_process(_name), iport1("iport1"), _func(_func)
            
    {
#ifdef FORSYDE_INTROSPECTION
        std::string func_name = std::string(basename());
        func_name = func_name.substr(0, func_name.find_last_not_of("0123456789")+1);
        arg_vec.push_back(std::make_tuple("_func",func_name+std::string("_func")));
        arg_vec.push_back(std::make_tuple("i1toks", std::to_string(1)));
#endif
    }
    
    //! Specifying from which process constructor is the module built
    std::string forsyde_kind() const {return "SADF::sink";}
    
private:

    // The current state of the process
    T* val;         

    //! The function passed to the process constructor
    functype _func;
    
    //Implementing the abstract semantics
    void init()
    {
        val = new T;
    }
    
    void prep()
    {
        *val = iport1.read();
    }
    
    void exec()
    {
        _func(*val);
    }
    
    void prod() {}
    
    void clean()
    {
        delete val;
    }
    
#ifdef FORSYDE_INTROSPECTION
    void bindInfo()
    {
        boundInChans.resize(1);    // only one output port
        boundInChans[0].port = &iport1;
    }
#endif
};

template <class T>
class source : public sadf_process
{
public:
    SADF_out<T> oport1;        ///< port for the output channel
    
    //! Type of the function to be passed to the process constructor
    typedef std::function<void(T&, const T&)> functype;

    //! The constructor requires the module name
    /*! It creates an SC_THREAD which runs the user-imlpemented function
     * and writes the result using the output port
     */
    source(const sc_module_name& _name,         ///< The module name
            const functype& _func,              ///< function to be passed
            const T& init_val,                  ///< Initial state
            const unsigned long long& take=0    ///< number of tokens produced (0 for infinite)
          ) : sadf_process(_name), oport1("oport1"),
              init_st(init_val), take(take), _func(_func)
    {
#ifdef FORSYDE_INTROSPECTION
        std::string func_name = std::string(basename());
        func_name = func_name.substr(0, func_name.find_last_not_of("0123456789")+1);
        arg_vec.push_back(std::make_tuple("_func",func_name+std::string("_func")));
        std::stringstream ss;
        ss << init_val;
        arg_vec.push_back(std::make_tuple("init_val", ss.str()));
        arg_vec.push_back(std::make_tuple("take", std::to_string(take)));
#endif
    }
    
    //! Specifying from which process constructor is the module built
    std::string forsyde_kind() const {return "UT::source";}
    
private:
    // The current state
    T init_st;   

    // Number of tokens produced     
    unsigned long long take;   

    // The current state of the process
    T* cur_st;       

    unsigned long long tok_cnt;
    bool infinite;
    
    //! The function passed to the process constructor
    functype _func;
    
    //Implementing the abstract semantics
    void init()
    {
        cur_st = new T;
        *cur_st = init_st;
        WRITE_MULTIPORT(oport1, *cur_st)
        if (take==0) infinite = true;
        tok_cnt = 1;
    }
    
    void prep() {}
    
    void exec()
    {
        _func(*cur_st, *cur_st);
    }
    
    void prod()
    {
        if (tok_cnt++ < take || infinite)
            WRITE_MULTIPORT(oport1, *cur_st)
        else wait();
    }
    
    void clean()
    {
        delete cur_st;
    }
    
#ifdef FORSYDE_INTROSPECTION
    void bindInfo()
    {
        boundOutChans.resize(1);    // only one output port
        boundOutChans[0].port = &oport1;
    }
#endif
};

}
}

#endif