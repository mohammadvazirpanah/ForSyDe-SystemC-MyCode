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

// template <class IT, class ST, class OT>
// class detector2 : public sadf_process
// {
// public:

//     SADF_in<IT> iport1;
//     SADF_out<OT> oport1;
//     SADF_out<OT> oport2;

//     typedef std::function<void(ST&, const ST&, const std::tuple<IT,IT>&)> ns_functype;

//     typedef std::function<void(std::tuple<IT,IT>&, const ST&, const std::tuple<IT,IT>&)> od_functype;

//     detector2(const sc_module_name& _name,
//             const ns_functype& _ns_func,
//             const od_functype& _od_func,
//             const ST& init_st  
//           ) : sadf_process(_name), _ns_func(_ns_func), _od_func(_od_func), 
//           init_st (init_st)
//     {
// #ifdef FORSYDE_INTROSPECTION

// #endif
//     }
    
//     //! Specifying from which process constructor is the module built
//     std::string forsyde_kind() const{return "SADF::detector2";}
    
// private:
   
//     ns_functype _ns_func;
//     od_functype _od_func;

//     ST init_st;

//     std::tuple<IT,IT> ivals;
//     ST* stval;
//     ST* nsval;
//     std::tuple<OT,OT> oval1;
//     std::tuple<OT,OT> oval2;

//     void init()
//     {
//         stval = new ST;
//         *stval = init_st;
//         nsval = new ST;
//     }
    
//     void prep()
//     {
//         ivals = iport1.read();
//     }
    
//     void exec()
//     {
//         _ns_func(*nsval, *stval, ivals);
//         _od_func(oval1, *stval, ivals);
//         _od_func(oval2, *stval, ivals);
//     }
    
//     void prod()
//     {


//     }
    
//     void clean()
//     {
//         delete stval;
//         delete nsval;
//     }
// #ifdef FORSYDE_INTROSPECTION
//     void bindInfo()
//     {
//         boundInChans.resize(1);     // only one input port
//         boundInChans[0].port = &iport1;
//         boundOutChans.resize(1);    // only one output port
//         boundOutChans[0].port = &oport1;
//     }
// #endif
// };



template <class TI, class TO>

class kernel : public sadf_process
{
public:

    typedef enum {SCENARIO_1, SCENARIO_2} scenario_type ;

    SADF_in<TI> iport1;
    SADF_in <scenario_type> control_port; 
    SADF_out<TO> oport1;


    typedef std::function<void
                              (std::vector<TO>&, 
                               const scenario_type&, 
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



}
}

#endif