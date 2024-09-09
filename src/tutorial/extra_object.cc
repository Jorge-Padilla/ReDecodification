#include "tutorial/extra_object.hh"

#include "base/trace.hh"
#include "debug/TutorialExample.hh"
#include "sim/sim_exit.hh"

namespace gem5
{

    ExtraObject::ExtraObject(const ExtraObjectParams &p) :
        SimObject(p),
        event([this]{processEvent();}, name()),
        //event(*this),
        buffer_size(p.buffer_size),
        write_bandwidth(p.write_bandwidth),
        buffer(nullptr),
        bufferUsed(0)
    {
        buffer = new char[buffer_size];
        DPRINTF(TutorialExample, "ExtraObject Created! Calles %s\n", name());
    }

    ExtraObject::~ExtraObject()
    {
        delete[] buffer;
    }

    void
    ExtraObject::processEvent()
    {
        DPRINTF(TutorialExample, "Processing the event of the ExtraObject!\n");
        fillBuffer();
    }

    void
    ExtraObject::print(std::string the_name)
    {
        DPRINTF(TutorialExample, "Printing to %0d !\n", the_name);
        message = the_name + "!! ";
        fillBuffer();
    }

    void
    ExtraObject::fillBuffer()
    {
        assert(message.length() > 0);

        //Copy per byte
        int bytes_copied = 0;
        for (auto i = message.begin(); i < message.end() &&
            bufferUsed < buffer_size - 1; i++, bufferUsed++, bytes_copied++) {
            buffer[bufferUsed] = *i;
        }

        if (bufferUsed < buffer_size - 1){
            DPRINTF(TutorialExample, "Filling next buffer in %0d ticks!\n",
                write_bandwidth * bytes_copied);
            schedule(event, curTick() + write_bandwidth * bytes_copied);
        } else {
            DPRINTF(TutorialExample, "Done Copying!\n");
            exitSimLoop(buffer, 0, curTick() + write_bandwidth * bytes_copied);
        }
    }

}   //namespace gem5
