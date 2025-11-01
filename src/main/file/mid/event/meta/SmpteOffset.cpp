#include "file/mid/event/meta/SmpteOffset.hpp"

#include "file/mid/event/MidiEvent.hpp"
#include "file/mid/event/meta/GenericMetaEvent.hpp"
#include "file/mid/event/meta/MetaEventData.hpp"
#include "file/mid/event/meta/MetaEvent.hpp"
#include "file/mid/event/meta/FrameRate.hpp"
#include "file/mid/util/VariableLengthInt.hpp"

using namespace mpc::file::mid::event::meta;

SmpteOffset::SmpteOffset(int tick, int delta, FrameRate *fps, int hour, int min,
                         int sec, int fr, int subfr)
    : MetaEvent(tick, delta, MetaEvent::SMPTE_OFFSET)
{
    mFrameRate = fps;
    mHours = hour;
    mMinutes = min;
    mSeconds = sec;
    mFrames = fr;
    mSubFrames = subfr;
    mLength = mpc::file::mid::util::VariableLengthInt(5);
}

const int SmpteOffset::FRAME_RATE_24;
const int SmpteOffset::FRAME_RATE_25;
const int SmpteOffset::FRAME_RATE_30_DROP;
const int SmpteOffset::FRAME_RATE_30;

void SmpteOffset::setFrameRate(FrameRate *fps)
{
    mFrameRate = fps;
}

FrameRate *SmpteOffset::getFrameRate()
{
    return mFrameRate;
}

void SmpteOffset::setHours(int h)
{
    mHours = h;
}

int SmpteOffset::getHours()
{
    return mHours;
}

void SmpteOffset::setMinutes(int m)
{
    mMinutes = m;
}

int SmpteOffset::getMinutes()
{
    return mMinutes;
}

void SmpteOffset::setSeconds(int s)
{
    mSeconds = s;
}

int SmpteOffset::getSeconds()
{
    return mSeconds;
}

void SmpteOffset::setFrames(int f)
{
    mFrames = f;
}

int SmpteOffset::getFrames()
{
    return mFrames;
}

void SmpteOffset::setSubFrames(int s)
{
    mSubFrames = s;
}

int SmpteOffset::getSubFrames()
{
    return mSubFrames;
}

int SmpteOffset::getEventSize()
{
    return 8;
}

void SmpteOffset::writeToOutputStream(std::ostream &out)
{
    MetaEvent::writeToOutputStream(out);
    out << (char)5;
    out << (char)mHours;
    out << (char)mMinutes;
    out << (char)mSeconds;
    out << (char)mFrames;
    out << (char)mSubFrames;
}

void SmpteOffset::writeToOutputStream(std::ostream &out, bool writeType)
{
    MetaEvent::writeToOutputStream(out, writeType);
}

std::shared_ptr<MetaEvent> SmpteOffset::parseSmpteOffset(int tick, int delta,
                                                         MetaEventData *info)
{
    if (info->length.getValue() != 5)
    {
        return std::make_shared<GenericMetaEvent>(tick, delta, info);
    }
    int rrHours = info->data[0];
    auto rr = rrHours >> 5;
    auto fps = FrameRate::fromInt(rr);
    auto hour = rrHours & 31;
    int min = info->data[1];
    int sec = info->data[2];
    int frm = info->data[3];
    int sub = info->data[4];
    return std::make_shared<SmpteOffset>(tick, delta, fps, hour, min, sec, frm,
                                         sub);
}

int SmpteOffset::compareTo(mpc::file::mid::event::MidiEvent *other)
{
    if (mTick != other->getTick())
    {
        return mTick < other->getTick() ? -1 : 1;
    }
    if (mDelta.getValue() != other->getDelta())
    {
        return mDelta.getValue() < other->getDelta() ? 1 : -1;
    }
    if (dynamic_cast<SmpteOffset *>(other) == nullptr)
    {
        return 1;
    }
    return 0;
}
