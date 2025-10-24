#pragma once
#include <midi/event/meta/MetaEvent.hpp>

namespace mpc::midi::event::meta
{

    class MetaEventData;

    class SequenceNumber
        : public MetaEvent
    {

    private:
        int mNumber;

    public:
        int getMostSignificantBits();
        int getLeastSignificantBits();
        int getSequenceNumber();

    public:
        static std::shared_ptr<MetaEvent> parseSequenceNumber(int tick, int delta, MetaEventData *info);

    public:
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;
        int getEventSize() override;

    public:
        int compareTo(mpc::midi::event::MidiEvent *other);

    public:
        SequenceNumber(int tick, int delta, int number);
    };
} // namespace mpc::midi::event::meta
