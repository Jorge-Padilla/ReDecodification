#include "tutorial/tutorial_object.hh"

#include "base/trace.hh"
#include "debug/TutorialExample.hh"

namespace gem5
{

    TutorialObject::TutorialObject(const TutorialObjectParams &p) :
        SimObject(p),
        event([this]{processEvent();}, name()),
        //event(*this),
        obj_name(name()),
        extra_object(p.extra_object),
        latency(p.latency),
        iterations(p.iterations)
    {
        panic_if(!extra_object,
            "TutorialObject must have a non-null ExtraObject");
        DPRINTF(TutorialExample,
            "TutorialObject Created! Called %s\n", obj_name);
    }

    void
    TutorialObject::processEvent()
    {
        iterations--;
        DPRINTF(TutorialExample,
            "Processing the event of the TutorialObject! %0d left\n",
            iterations);

        if (iterations <= 0) {
            DPRINTF(TutorialExample, "No More Iterations!\n");
            extra_object->print(obj_name);
        } else {
            schedule(event, curTick() + latency);
        }
    }

    void
    TutorialObject::startup()
    {
        schedule(event, latency);
    }

}   //namespace gem5
