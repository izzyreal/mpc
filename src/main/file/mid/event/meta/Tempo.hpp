#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

namespace mpc::file::mid::event::meta
{
    class MetaEventData;
}

namespace mpc::file::mid::event::meta
{
    class Tempo : public MetaEvent
    {

    public:
        static const int DEFAULT_MPQN{500000};

    private:
        int mMPQN;
        float mBPM;

    public:
        float getBpm() const;
        void setMpqn(int m);
        void setBpm(float b);

    public:
        int getEventSize() override;
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;

    public:
        static std::shared_ptr<MetaEvent> parseTempo(int tick, int delta,
                                                     MetaEventData *info);

    public:
        virtual int compareTo(mpc::file::mid::event::MidiEvent *other);

    public:
        Tempo();
        Tempo(int tick, int delta, int mpqn);
    };
} // namespace mpc::file::mid::event::meta
