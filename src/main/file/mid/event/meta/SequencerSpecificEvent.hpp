#pragma once

#include "file/mid/event/meta/MetaEvent.hpp"

namespace mpc::file::mid::event::meta
{

    class SequencerSpecificEvent : public MetaEvent
    {
        std::vector<char> mData;

    public:
        virtual void setData(std::vector<char> data);
        virtual std::vector<char> getData();

        int getEventSize() override;
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;

        virtual int compareTo(MidiEvent *other);

        SequencerSpecificEvent(int tick, int delta,
                               const std::vector<char> &data);
    };
} // namespace mpc::file::mid::event::meta
