/**********************************************************************           
    * selfaware_process_constructors.hpp                              *
    *                                                                 *
    *                                                                 *
    * Author:  Mohammad Vazirpanah (mohammad.vazirpanah@yahoo.com)    *
    *                                                                 *
    * Purpose: Providing basic process constructors for modeling      *
    *          selfaware systems in ForSyDe-SystemC                   *
    *                                                                 *
    * Usage:   This file is included automatically                    *
    *                                                                 *
    * License: BSD3                                                   *
    *******************************************************************/

#ifndef SELFAWARE_PROCESS_CONSTRUCTORS_HPP
#define SELFAWARE_PROCESS_CONSTRUCTORS_HPP

#include "sadf_moc.hpp"
#include "sdf_moc.hpp"


/*! \file selfaware_process_constructors.hpp
 * \brief Implements the basic process constructors in the self-aware system
 * 
 *  This file includes the basic process constructors used for modeling
 * in the self-aware model.
 */




namespace ForSyDe
{

namespace SADF
{

template<typename TO_tuple, typename TC, typename TI_tuple> class SA;
template <typename... TOs, typename TC, typename... TIs>
class SA<std::tuple<TOs...>,TC,std::tuple<TIs...>> : public kernelMN<std::tuple<TOs...>,TC,std::tuple<TIs...>>

{
public:

    typedef std::function<void(std::vector<bool>& )>func_goal;

    SA(sc_module_name _name,    
        const typename kernelMN<std::tuple<TOs...>,TC,std::tuple<TIs...>>::functype& _func,
        const typename kernelMN<std::tuple<TOs...>,TC,std::tuple<TIs...>>::scenario_table_type& scenario_table,
        const func_goal& _goals
        ) : kernelMN<std::tuple<TOs...>,TC,std::tuple<TIs...>>(_name, _func, scenario_table), _goals(_goals)
    {}
    private: 
    func_goal _goals; 

};


template<typename TO_tuple, typename TI_tuple, typename TS> class simulator;
template <typename... TOs, typename... TIs, typename TS>
class simulator<std::tuple<TOs...>,std::tuple<TIs...>,TS> : public detectorMN<std::tuple<TOs...>,std::tuple<TIs...>,TS>

{
    public: 

    simulator(sc_module_name _name,                
            const typename detectorMN<std::tuple<TOs...>,std::tuple<TIs...>,TS>::cds_functype& _cds_func,            
            const typename detectorMN<std::tuple<TOs...>,std::tuple<TIs...>,TS>::kss_functype& _kss_func,           
            const typename detectorMN<std::tuple<TOs...>,std::tuple<TIs...>,TS>::scenario_table_type& scenario_table,
            const TS& init_sc,                        
            const std::array<size_t,sizeof...(TIs)>& itoks    
            ) : detectorMN<std::tuple<TOs...>,std::tuple<TIs...>,TS> (_name, _cds_func, _kss_func, scenario_table, init_sc, itoks)
          {}

};


template <typename T0, typename T1>
class environment : public SDF::comb<T0, T1>
{

public:

    environment(sc_module_name _name,      
        const typename SDF::comb<T0, T1>::functype _func,           
        unsigned int o1toks,      
        unsigned int i1toks       
        ) : SDF::comb<T0, T1>(_name, _func, o1toks, i1toks)
    {}


};


template <typename T0, typename TC, typename T1>
class input_abstraction : public kernel<T0, TC, T1>
{

public:

    input_abstraction(sc_module_name _name,            
        const typename kernel<T0, TC, T1>::functype& _func,             
        const typename kernel<T0, TC, T1>::scenario_table_type& scenario_table
        ) : kernel<T0, TC, T1> (_name, scenario_table, _func)
    {}
private: 

};


template <typename T0, typename TC, typename T1>
class output_abstraction : public kernel<T0, TC, T1>
{

public:

    output_abstraction(sc_module_name _name,            
        const typename kernel<T0, TC, T1>::functype& _func,             
        const typename kernel<T0, TC, T1>::scenario_table_type& scenario_table
        ) : kernel<T0, TC, T1> (_name, scenario_table, _func)
    {}
private: 

};



class SA_monitor
{

};

}
}
#endif