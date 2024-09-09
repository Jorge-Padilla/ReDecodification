#ifndef __TUTORIAL_EXTRA_OBJECT_HH__
#define __TUTORIAL_EXTRA_OBJECT_HH__

#include <string>

#include "params/ExtraObject.hh"
#include "sim/sim_object.hh"

namespace gem5
{

class ExtraObject : public SimObject
{
    private:
        void processEvent();

        //Fills the buffer in one iteration, or enqueue another event
        void fillBuffer();

        EventFunctionWrapper event;

        int buffer_size;

        float write_bandwidth;

        char *buffer;

        int bufferUsed;

        std::string message;

    public:
        ExtraObject(const ExtraObjectParams &p);

        ~ExtraObject();

        void print(std::string the_name);
};

} // namespace gem5

#endif // __TUTORIAL_EXTRA_OBJECT_HH__
