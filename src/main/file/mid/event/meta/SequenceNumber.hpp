#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

namespace mpc::file::mid::event::meta
{

    class MetaEventData;

    class SequenceNumber : public MetaEvent
    {

        int mNumber;

    public:
        int getMostSignificantBits() const;
        int getLeastSignificantBits() const;
        int getSequenceNumber() const;

        static std::shared_ptr<MetaEvent>
        parseSequenceNumber(int tick, int delta, MetaEventData *info);

        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;
        int getEventSize() override;

        int compareTo(MidiEvent *other) const;

        SequenceNumber(int tick, int delta, int number);
    };
} // namespace mpc::file::mid::event::meta
