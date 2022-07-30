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

#include "sadf_process.hpp"

namespace ForSyDe
{

namespace SADF
{

using namespace sc_core;

template <class IT, class OT>
class detector2 : public sadf_process
{
public:

    SADF_in<IT> iport1;
    SADF_out<std::tuple<OT,OT>> oport1;



    detector2(const sc_module_name& _name
          ) : sadf_process(_name)

{

#ifdef FORSYDE_INTROSPECTION

#endif
}
    
    //! Specifying from which process constructor is the module built
    std::string forsyde_kind() const{return "SADF::detector2";}
    
private:
   
    IT ival;
    std::tuple<OT,OT> ovals;

    void init()
    {
        std::cout<<"init"<<std::endl;
    }
    
    void prep()
    {

        // ival = iport1.read();

    }
    
    void exec()
    {

    }
    
    void prod()
    {
        ovals = std::make_tuple(1,2); //send scenario to kernel
        WRITE_MULTIPORT (oport1, ovals);
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



template <class TI, class TCT, class TO>

class kernel : public sadf_process
{
public:

    typedef enum {SCENARIO_1, SCENARIO_2} scenario_type ;

    SADF_in<TI> iport1;
    SADF_in <TCT> control_port; 
    SADF_out<TO> oport1;


    typedef std::function<void
                              (std::vector<TO>&, 
                               const TCT&, 
                               const std::vector<TI>&)
                               > kernel_functype;


    kernel(const sc_module_name& _name,
           const kernel_functype& _func
          ) : sadf_process(_name), iport1("iport1"), control_port("control_port"), oport1("oport1"), _func(_func)
    {
#ifdef FORSYDE_INTROSPECTION


#endif
    }
    
    //! Specifying from which process constructor is the module built
    std::string forsyde_kind() const{return "SADF::kernel";}
    
private:
   
    kernel_functype _func;

    size_t o1toks, i1toks;

    std::vector<TO> o1vals;
    std::vector<TI> i1vals;
    // scenario_type scenario_val;
    scenario_type scenario_val;
 

    void init()
    {

    }
    
    void prep()
    {
        

        scenario_val = control_port.read();
        
    }
    
    void exec()
    {
        _func(o1vals, scenario_val ,i1vals); 
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

}
}

#endif