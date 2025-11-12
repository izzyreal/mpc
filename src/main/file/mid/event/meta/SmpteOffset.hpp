#pragma once
#include "file/mid/event/meta/MetaEvent.hpp"

namespace mpc::file::mid::event::meta
{

    class FrameRate;

    class MetaEventData;

    class SmpteOffset : public MetaEvent
    {

    public:
        static const int FRAME_RATE_24{0};
        static const int FRAME_RATE_25{1};
        static const int FRAME_RATE_30_DROP{2};
        static const int FRAME_RATE_30{3};

    private:
        FrameRate *mFrameRate{};
        int mHours{};
        int mMinutes{};
        int mSeconds{};
        int mFrames{};
        int mSubFrames{};

    public:
        virtual void setFrameRate(FrameRate *fps);
        virtual FrameRate *getFrameRate();
        virtual void setHours(int h);
        virtual int getHours();
        virtual void setMinutes(int m);
        virtual int getMinutes();
        virtual void setSeconds(int s);
        virtual int getSeconds();
        virtual void setFrames(int f);
        virtual int getFrames();
        virtual void setSubFrames(int s);
        virtual int getSubFrames();

        int getEventSize() override;
        void writeToOutputStream(std::ostream &out) override;
        void writeToOutputStream(std::ostream &out, bool writeType) override;

        static std::shared_ptr<MetaEvent> parseSmpteOffset(int tick, int delta,
                                                           MetaEventData *info);
        virtual int compareTo(MidiEvent *other);

        SmpteOffset(int tick, int delta, FrameRate *fps, int hour, int min,
                    int sec, int fr, int subfr);
    };
} // namespace mpc::file::mid::event::meta
