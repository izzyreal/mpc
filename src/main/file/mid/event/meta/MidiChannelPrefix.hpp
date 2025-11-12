#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

namespace mpc::file::mid::event::meta
{
    class MetaEventData;
}

namespace mpc::file::mid::event::meta
{
    class MidiChannelPrefix : public MetaEvent
    {

        int mChannel;

    public:
        virtual void setChannel(int c);
        virtual int getChannel();

        int getEventSize() override;

        void writeToOutputStream(std::ostream &out) override;
        static std::shared_ptr<MetaEvent>
        parseMidiChannelPrefix(int tick, int delta, MetaEventData *info);
        virtual int compareTo(MidiEvent *other);

        MidiChannelPrefix(int tick, int delta, int channel);
    };
} // namespace mpc::file::mid::event::meta
