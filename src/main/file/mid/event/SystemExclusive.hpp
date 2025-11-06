#pragma once
#include "file/mid/event/MidiEvent.hpp"

#include "file/mid/util/VariableLengthInt.hpp"

namespace mpc::file::mid::event
{

    class SystemExclusiveEvent : public MidiEvent
    {

    private:
        int mType;
        mpc::file::mid::util::VariableLengthInt *mLength;
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
        SystemExclusiveEvent(int type, int tick, const std::vector<char> &data);
        SystemExclusiveEvent(int type, int tick, int delta,
                             const std::vector<char> &data);
    };
} // namespace mpc::file::mid::event
