#ifndef __TUTORIAL_TUTORIAL_OBJECT_HH__
#define __TUTORIAL_TUTORIAL_OBJECT_HH__

#include <string>

#include "params/TutorialObject.hh"
#include "sim/sim_object.hh"
#include "tutorial/extra_object.hh"

namespace gem5
{

class TutorialObject : public SimObject
{
    private:
        void processEvent();

        EventFunctionWrapper event;

        const std::string obj_name;

        const Tick latency;

        int iterations;

        ExtraObject *extra_object;

    public:
        TutorialObject(const TutorialObjectParams &p);

        void startup() override;
};

} // namespace gem5

#endif // __TUTORIAL_TUTORIAL_OBJECT_HH__
