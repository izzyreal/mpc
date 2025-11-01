#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

namespace mpc::file::mid::event::meta
{

    class MetaEventData;

    class GenericMetaEvent : public MetaEvent
    {

    private:
        std::vector<char> mData;

    public:
        int getEventSize() override;
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;

    public:
        virtual int compareTo(mpc::file::mid::event::MidiEvent *other);

    public:
        GenericMetaEvent(int tick, int delta, MetaEventData *info);
    };
} // namespace mpc::file::mid::event::meta
