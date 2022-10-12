

#include <forsyde.hpp>
#include <ros/ros.h>
#include "ASA.hpp"

#include "forsyde/ros_sadf_wrapper.hpp"





using namespace sc_core;
using namespace ForSyDe;
using namespace std;



SC_MODULE(top)
{

    SADF::signal<> from_robot1, from_robot2;
    SADF::signal<> to_robot1, to_robot2;

    SC_CTOR(top)
    {
        
        //! ------------------ Environment ------------------
        SADF::make_roswrap("roswrap", 
                from_robot_sensor1,
                from_robot_sensor2, 
                to_robot_sensor1,
                to_robot_sensor2
                );

        //! ---------------------- ASA -----------------------
        auto ASA1 = new ASA("ASA1");

        ASA1->iport1(from_robot_sensor1);
        ASA1->iport2(from_robot_sensor2);
        ASA1->oport1(to_robot_sensor1);
        ASA1->oport2(to_robot_sensor2);    

    
    }   


#ifdef FORSYDE_INTROSPECTION
    void start_of_simulation()
    {
        ForSyDe::XMLExport dumper("gen/");
        dumper.traverse(this);
    }
#endif

};

