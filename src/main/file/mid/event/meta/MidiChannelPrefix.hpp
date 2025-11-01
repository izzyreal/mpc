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

    private:
        int mChannel;

    public:
        virtual void setChannel(int c);
        virtual int getChannel();

    public:
        int getEventSize() override;

    public:
        void writeToOutputStream(std::ostream &out) override;
        static std::shared_ptr<MetaEvent>
        parseMidiChannelPrefix(int tick, int delta, MetaEventData *info);
        virtual int compareTo(mpc::file::mid::event::MidiEvent *other);

        MidiChannelPrefix(int tick, int delta, int channel);
    };
} // namespace mpc::file::mid::event::meta
