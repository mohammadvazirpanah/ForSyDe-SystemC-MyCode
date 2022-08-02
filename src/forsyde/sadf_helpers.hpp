#ifndef SADF_HELPERS_HPP
#define SADF_HELPERS_HPP

/*! \file sadf_helpers.hpp
 * \brief Implements helper primitives for modeling in the SADF MoC
 * 
 *  This file includes helper functions which facilliate construction of
 * processes in the SADF MoC
 */

#include <functional>
#include <tuple>

#include "sadf_process_constructors.hpp"

namespace ForSyDe
{

namespace SADF
{

using namespace sc_core;


//! Helper function to construct a constant source process
/*! This function is used to construct a constant (SystemC module) and
 * connect its output signal.
 * It provides a more functional style definition of a ForSyDe process.
 * It also removes bilerplate code by using type-inference feature of
 * C++ and automatic binding to the output FIFOs.
 */
template <class T, template <class> class OIf>
inline constant<T>* make_constant(std::string pName,
    T initval,
    unsigned long long take,
    OIf<T>& outS
    )
{
    auto p = new constant<T>(pName.c_str(), initval, take);
    
    (*p).oport1(outS);
    
    return p;
}


//! Helper function to construct a source process
/*! This function is used to construct a source (SystemC module) and
 * connect its output signal.
 * It provides a more functional style definition of a ForSyDe process.
 * It also removes bilerplate code by using type-inference feature of
 * C++ and automatic binding to the output FIFOs.
 */
template <class T, template <class> class OIf>
inline source<T>* make_source(std::string pName,
    typename source<T>::functype _func,
    T initval,
    unsigned long long take,
    OIf<T>& outS
    )
{
    auto p = new source<T>(pName.c_str(), _func, initval, take);
    
    (*p).oport1(outS);
    
    return p;
}

//! Helper function to construct a sink process
/*! This function is used to construct a sink (SystemC module) and
 * connect its output and output signals.
 * It provides a more functional style definition of a ForSyDe process.
 * It also removes bilerplate code by using type-inference feature of
 * C++ and automatic binding to the input FIFOs.
 */
template <class T, template <class> class IIf>
inline sink<T>* make_sink(std::string pName,
    typename sink<T>::functype _func,
    IIf<T>& inS
    )
{
    auto p = new sink<T>(pName.c_str(), _func);
    
    (*p).iport1(inS);
    
    return p;
}

//! Helper function to construct a detector12 process
/*! This function is used to construct a detector12 (SystemC module) and
 * connect its output and output signals.
 * It provides a more functional style definition of a ForSyDe process.
 * It also removes bilerplate code by using type-inference feature of
 * C++ and automatic binding to the input FIFOs.
 */

template <typename IT, typename ST, typename OT,
           template <class> class IIf,
           template <class> class OI1f,
           template <class> class OI2f>
inline detector12<IT,ST,OT>* make_detector12(const std::string& pName,
    const typename detector12<IT,ST,OT>::gamma_functype& _gamma_func,
    const typename detector12<IT,ST,OT>::ns_functype& _ns_func,
    const typename detector12<IT,ST,OT>::od_functype& _od_func,
    const ST& init_st,
    OI1f<OT>& outS1,
    OI2f<OT>& outS2,
    IIf<IT>& inpS
    )
{
    auto p = new detector12<IT,ST,OT>(pName.c_str(), _gamma_func, _ns_func, _od_func, init_st);
    
    (*p).iport1(inpS);
    (*p).oport1(outS1);
    (*p).oport2(outS2);

    
    return p;
}



//! Helper function to construct a kernel process
/*! This function is used to construct a kernel (SystemC module) and
 * connect its output and output signals.
 * It provides a more functional style definition of a ForSyDe process.
 * It also removes bilerplate code by using type-inference feature of
 * C++ and automatic binding to the input FIFOs.
 */

template <typename IT, typename TST, typename OT,
           template <class> class I1If,
           template <class> class I2If,
           template <class> class OIf>
inline kernel<IT,TST,OT>* make_kernel(const std::string& pName,
    const typename kernel<IT,TST,OT>::kernel_functype& _func,
    const typename kernel<IT,TST,OT>::scenario_table& _scenario_table,
    OIf<OT>& outS1,
    I1If<IT>& inpS1,
    I2If<TST>& inpS2
    )
    
{
    auto p = new kernel<IT,TST,OT>(pName.c_str(), _func, _scenario_table);
    
    (*p).iport1(inpS1);
    (*p).control_port(inpS2);
    (*p).oport1(outS1);
    
    return p;
}

}
}

#endif
