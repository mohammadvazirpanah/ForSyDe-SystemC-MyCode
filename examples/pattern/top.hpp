

#include <forsyde.hpp>
#include "forsyde/pattern_constructor.hpp"

using namespace sc_core;
using namespace ForSyDe;
using namespace std;



SC_MODULE(top)
{




    SC_CTOR(top)
    {
       
        
    }   


#ifdef FORSYDE_INTROSPECTION
    void start_of_simulation()
    {
        ForSyDe::XMLExport dumper("gen/");
        dumper.traverse(this);
    }
#endif

};

