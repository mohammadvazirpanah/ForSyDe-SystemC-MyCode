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


template <class T>
class constant : public sadf_process
{
public:
    SADF_out<T> oport1;            ///< port for the output channel

    //! The constructor requires the module name
    /*! It creates an SC_THREAD which runs the user-imlpemented function
     * and writes the result using the output port
     */
    constant(sc_module_name _name,      ///< The module name
              T init_val,                ///< The constant output value
              unsigned long long take=0 ///< number of tokens produced (0 for infinite)
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
    T init_val;
    unsigned long long take;    // Number of tokens produced
    
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


template <class TI, class TS, class TO>
class detector12 : public sadf_process
{
public:

    SADF_in<TI>  iport1;   // input port
    SADF_out<TO> oport1;   // output port
    SADF_out<TO> oport2;   // output port
    
    //! Type of the decoding output rate function to be passed to the process constructor
    typedef std::function<void(std::array<size_t,2>&, 
                                const TS&)> gamma_functype;
    
    // //! Type of the next-state function to be passed to the process constructor
    // typedef std::function<void(TS&,
    //                             const TS&,
    //                             const std::vector<TI>&)> ns_functype;
    
    //! Type of the output-decoding function to be passed to the process constructor
    typedef std::function<void(std::array<TO,2>&,
                                const TS&,
                                const std::vector<TI>&)> od_functype;
    
    //! The constructor requires the module name
    /*! It creates an SC_THREAD which reads data from its input port,
     * applies the user-imlpemented functions to the input and current
     * state and writes the results using the output port
     */
    detector12(const sc_module_name& _name,      ///< The module name
            const gamma_functype& _gamma_func,  ///< The partitioning function
            // const ns_functype& _ns_func,        ///< The next_state function
            const od_functype& _od_func,        ///< The output-decoding function
            const TS& init_st,                  ///< Initial state
            const unsigned int& itoks           ///< consumption rate for the first input
            ) : sadf_process(_name), _gamma_func(_gamma_func),
              _od_func(_od_func), init_st(init_st), itoks(itoks)
    {
#ifdef FORSYDE_INTROSPECTION
        std::string func_name = std::string(basename());
        func_name = func_name.substr(0, func_name.find_last_not_of("0123456789")+1);
        arg_vec.push_back(std::make_tuple("_gamma_func",func_name+std::string("_gamma_func")));
        arg_vec.push_back(std::make_tuple("_ns_func",func_name+std::string("_ns_func")));
        arg_vec.push_back(std::make_tuple("_od_func",func_name+std::string("_od_func")));
        std::stringstream ss;
        ss << init_st;
        arg_vec.push_back(std::make_tuple("init_st",ss.str()));
#endif
    }
    
    //! Specifying from which process constructor is the module built
    std::string forsyde_kind() const{return "SADF::detector12";}
    
private:
    //! The functions passed to the process constructor
    gamma_functype _gamma_func;
    // ns_functype _ns_func;
    od_functype _od_func;
    TS init_st;
    unsigned int itoks;
    std::array<size_t,2> out_rates;
    
    // Input, output, current state, and next state variables
    std::array<TO,2> ovals;
    std::vector<TI> ivals;
    TS* stvals;
    // TS* nsvals;

    //Implementing the abstract semantics
    void init()
    {

        ivals.resize(itoks);
        stvals = new TS;
        *stvals = init_st;
        // nsvals = new TS;
    }
    
    void prep()
    {
        for (auto it=ivals.begin();it!=ivals.end();it++)
            *it = iport1.read();

    }

    void exec()
    {

         _gamma_func(out_rates, *stvals);
        _od_func(ovals, *stvals, ivals);
        // _ns_func(*nsvals, *stvals, ivals);
    }
    
    void prod()
    {   
        for (auto it=0; it<out_rates[0]; it++)
            oport1.write(ovals[0]);

        for (auto it=0; it<out_rates[1]; it++)
            oport2.write(ovals[1]);
    }
    
    void clean()
    {
        
        delete stvals;
        // delete nsvals;
        
    }
#ifdef FORSYDE_INTROSPECTION
    void bindInfo()
    {
        boundInChans.resize(sizeof...(TIs));     // input ports
        std::apply
        (
            [&](auto&... ports)
            {
                std::size_t n{0};
                ((boundInChans[n++].port = &ports),...);
            }, iport
        );
        boundOutChans.resize(sizeof...(TOs));    // output ports
        std::apply
        (
            [&](auto&... ports)
            {
                std::size_t n{0};
                ((boundOutChans[n++].port = &ports),...);
            }, oport
        );
    }
#endif
};



template <class TI, class TCT, class TO>

class kernel21 : public sadf_process
{
public:

    SADF_in<TI> iport1;
    SADF_in <TCT> control_port; 
    SADF_out<TO> oport1;

    typedef std::map<TCT,std::tuple<std::array<size_t,1>,std::array<size_t,1>>> scenario_table;

    typedef std::function<void(
                               std::vector<TO>&, 
                               const TCT&, 
                               scenario_table&,
                               const std::vector<TI>&
                               )> kernel_functype;
                               
    kernel21(const sc_module_name& _name,
           const kernel_functype& _func,
           const scenario_table& _scenario_table
          ) : sadf_process(_name), iport1("iport1"), control_port("control_port"), oport1("oport1"), _func(_func),
                _scenario_table (_scenario_table)
    {
#ifdef FORSYDE_INTROSPECTION


#endif
    }
    
    //! Specifying from which process constructor is the module built
    std::string forsyde_kind() const{return "SADF::kernel21";}
    
private:
   
    kernel_functype _func;
    scenario_table _scenario_table;
    size_t o1toks, i1toks;
    TCT cntl_port; 
    std::vector<TO> o1vals;
    std::vector<TI> i1vals;

    size_t cons_rate;
    size_t prod_rate;
    void init()
    {

    }
    
    void prep()
    {

        cntl_port = control_port.read();

        cons_rate = std::get<0>(_scenario_table[cntl_port])[0];
        prod_rate = std::get<1>(_scenario_table[cntl_port])[0];

        i1vals.resize(cons_rate);
        o1vals.resize(prod_rate);
        
        for (auto it=i1vals.begin();it!=i1vals.end();it++)
        {
            *it = iport1.read();
            std::cout<<"i1vals: "<<*it<<std::endl;
        }
            

    }
    
    void exec()
    {
        _func(o1vals, cntl_port, _scenario_table, i1vals); 
    }
    
    void prod()
    {

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
    T* val;         // The current state of the process

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
    source(const sc_module_name& _name,   ///< The module name
            const functype& _func,         ///< function to be passed
            const T& init_val,              ///< Initial state
            const unsigned long long& take=0 ///< number of tokens produced (0 for infinite)
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
    T init_st;        // The current state
    unsigned long long take;    // Number of tokens produced
    
    T* cur_st;        // The current state of the process
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