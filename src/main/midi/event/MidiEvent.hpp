#pragma once

#include <midi/util/VariableLengthInt.hpp>

#include <string>
#include <memory>
#include <fstream>
#include <sstream>

namespace mpc::midi::event
{
    class MidiEvent
    {
    public:
        int mTick = 0;
        mpc::midi::util::VariableLengthInt mDelta;

    public:
        virtual int getTick();
        virtual int getDelta();
        virtual void setDelta(int d);

    public:
        virtual int getEventSize() = 0;

    public:
        virtual int getSize();
        virtual bool requiresStatusByte(MidiEvent *prevEvent);
        virtual void writeToOutputStream(std::ostream &out, bool writeType);

    private:
        static int sId;
        static int sType;
        static int sChannel;

    public:
        static std::shared_ptr<MidiEvent> parseEvent(int tick, int delta, std::stringstream &in);

    private:
        static bool verifyIdentifier(int id);

    public:
        virtual std::string toString();

        MidiEvent(int tick, int delta);
    };
} // namespace mpc::midi::event
