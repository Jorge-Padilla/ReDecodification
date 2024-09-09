#ifndef __TUTORIAL_TUTORIAL_OBJECT_HH__
#define __TUTORIAL_TUTORIAL_OBJECT_HH__

#include "params/TutorialObject.hh"
#include "sim/sim_object.hh"

namespace gem5
{

class TutorialObject : public sim_object
{
    public:
        TutorialObject(const TutorialObjectParams &p);

}

} // namespace gem5

#endif // __TUTORIAL_TUTORIAL_OBJECT_HH__
