#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

#include <string>

namespace mpc::file::mid::event::meta
{
    class TextualMetaEvent : public MetaEvent
    {

    public:
        std::string mText;

    public:
        void setText(std::string t);
        std::string getText();

    public:
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;
        std::string toString() override;
        int getEventSize() override;

    public:
        int compareTo(mpc::file::mid::event::MidiEvent *other);

    public:
        TextualMetaEvent(int tick, int delta, int type, std::string text);
    };
} // namespace mpc::file::mid::event::meta
