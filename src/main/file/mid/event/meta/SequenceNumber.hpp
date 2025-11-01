#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

namespace mpc::file::mid::event::meta
{

    class MetaEventData;

    class SequenceNumber : public MetaEvent
    {

    private:
        int mNumber;

    public:
        int getMostSignificantBits();
        int getLeastSignificantBits();
        int getSequenceNumber();

    public:
        static std::shared_ptr<MetaEvent>
        parseSequenceNumber(int tick, int delta, MetaEventData *info);

    public:
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;
        int getEventSize() override;

    public:
        int compareTo(mpc::file::mid::event::MidiEvent *other);

    public:
        SequenceNumber(int tick, int delta, int number);
    };
} // namespace mpc::file::mid::event::meta
