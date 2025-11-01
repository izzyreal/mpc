#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

namespace mpc::file::mid::event::meta
{
    class MetaEventData;
}

namespace mpc::file::mid::event::meta
{
    class TimeSignature : public MetaEvent
    {

    public:
        static const int METER_EIGHTH{12};
        static const int METER_QUARTER{24};
        static const int METER_HALF{48};
        static const int METER_WHOLE{96};
        static const int DEFAULT_METER{24};
        static const int DEFAULT_DIVISION{8};

    private:
        int mNumerator;
        int mDenominator;
        int mMeter;
        int mDivision;

    public:
        void setTimeSignature(int num, int den, int meter, int div);
        int getNumerator();
        int getDenominatorValue();
        int getRealDenominator();
        int getMeter();
        int getDivision();

    public:
        int getEventSize() override;
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;

    public:
        static std::shared_ptr<MetaEvent>
        parseTimeSignature(int tick, int delta, MetaEventData *info);

    private:
        int log2(int den);

    public:
        std::string toString() override;
        virtual int compareTo(mpc::file::mid::event::MidiEvent *other);

    public:
        TimeSignature();
        TimeSignature(int tick, int delta, int num, int den, int meter,
                      int div);
    };
} // namespace mpc::file::mid::event::meta
