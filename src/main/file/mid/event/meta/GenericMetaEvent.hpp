#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

namespace mpc::file::mid::event::meta
{

    class MetaEventData;

    class GenericMetaEvent : public MetaEvent
    {

        std::vector<char> mData;

    public:
        int getEventSize() override;
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;

        virtual int compareTo(MidiEvent *other);

        GenericMetaEvent(int tick, int delta, MetaEventData *info);
    };
} // namespace mpc::file::mid::event::meta
