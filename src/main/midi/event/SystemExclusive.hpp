#pragma once
#include <midi/event/MidiEvent.hpp>

#include <midi/util/VariableLengthInt.hpp>

namespace mpc::midi::event
{

    class SystemExclusiveEvent
        : public MidiEvent
    {

    private:
        int mType;
        mpc::midi::util::VariableLengthInt *mLength;
        std::vector<char> mData;

    public:
        virtual std::vector<char> getData();
        virtual void setData(std::vector<char> data);
        bool requiresStatusByte(MidiEvent *prevEvent) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;
        virtual int compareTo(MidiEvent *other);

    public:
        int getEventSize() override;

    public:
        SystemExclusiveEvent(int type, int tick, std::vector<char> data);
        SystemExclusiveEvent(int type, int tick, int delta, std::vector<char> data);
    };
} // namespace mpc::midi::event
