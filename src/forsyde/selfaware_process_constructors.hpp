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


template<typename TO_tuple, typename TC, typename TI_tuple> class system;
template <typename... TOs, typename TC, typename... TIs>
class system<std::tuple<TOs...>,TC,std::tuple<TIs...>> : public kernelMN<std::tuple<TOs...>,TC,std::tuple<TIs...>>

{
public:

    typedef std::map<TC,std::tuple<
                        std::array<size_t,sizeof...(TIs)>,
                        std::array<size_t,sizeof...(TOs)>
                    >> scenario_table_type;

    typedef std::function<void(
                                std::tuple<std::vector<TOs>...>&,
                                const TC&,
                                const std::tuple<std::vector<TIs>...>&
                            )> functype;


    typedef std::function<void(std::vector<bool>& )>func_goal;


    system(sc_module_name _name,    
        const functype& _func,
        const scenario_table_type& scenario_table,
        const func_goal& _goals
        ) : kernelMN<std::tuple<TOs...>,TC,std::tuple<TIs...>>(_name, _func, scenario_table), _goals(_goals)
    {}
    private: 
    func_goal _goals; 

    // void exec()
    // {
        
    //     kernelMN<std::tuple<TOs...>,TC,std::tuple<TIs...>>::exec();

    // }

};


template<typename TO_tuple, typename TI_tuple, typename TS> class simulator;
template <typename... TOs, typename... TIs, typename TS>
class simulator<std::tuple<TOs...>,std::tuple<TIs...>,TS> : public detectorMN<std::tuple<TOs...>,std::tuple<TIs...>,TS>

{
    public: 

    typedef std::map<TS,std::array<size_t,sizeof...(TOs)>> scenario_table_type;

    typedef std::function<void(TS&,
                                const TS&,
                                const std::tuple<std::vector<TIs>...>&)> cds_functype;

    typedef std::function<void(std::tuple<TOs...>&,
                                const TS&,
                                const std::tuple<std::vector<TIs>...>&)> kss_functype;

    simulator(sc_module_name _name,                
          const cds_functype& _cds_func,            
          const kss_functype& _kss_func,           
          const scenario_table_type& scenario_table,
          const TS& init_sc,                        
          const std::array<size_t,sizeof...(TIs)>& itoks    
          ) : detectorMN<std::tuple<TOs...>,std::tuple<TIs...>,TS> (_name, _cds_func, _kss_func, scenario_table, init_sc, itoks)
          {}

    

};


template <typename T0, typename T1>
class environment : public SDF::comb<T0, T1>
{

public:

    typedef std::function<void(std::vector<T0>&,
                                const std::vector<T1>&
                                )> functype;

    environment(sc_module_name _name,      
         functype _func,           
         unsigned int o1toks,      
         unsigned int i1toks       
         ) : SDF::comb<T0, T1>(_name, _func, o1toks, i1toks)
    {}
private: 


};
}
}

#endif