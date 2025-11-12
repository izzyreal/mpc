#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

#include <string>

namespace mpc::file::mid::event::meta
{
    class TextualMetaEvent : public MetaEvent
    {

    public:
        std::string mText;

        void setText(const std::string &t);
        std::string getText();

        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;
        std::string toString() override;
        int getEventSize() override;

        int compareTo(MidiEvent *other) const;

        TextualMetaEvent(int tick, int delta, int type,
                         const std::string &text);
    };
} // namespace mpc::file::mid::event::meta
