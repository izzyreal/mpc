#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

namespace mpc::file::mid::event::meta
{
    class MetaEventData;

    class KeySignature : public MetaEvent
    {

    public:
        static const int SCALE_MAJOR{0};
        static const int SCALE_MINOR{1};

    private:
        int mKey;
        int mScale;

    public:
        void setKey(int key);
        int getKey();
        void setScale(int scale);
        int getScale();

    public:
        int getEventSize() override;
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;

    public:
        static std::shared_ptr<MetaEvent>
        parseKeySignature(int tick, int delta, MetaEventData *info);
        int compareTo(mpc::file::mid::event::MidiEvent *other);

    public:
        KeySignature(int tick, int delta, int key, int scale);
    };
} // namespace mpc::file::mid::event::meta
