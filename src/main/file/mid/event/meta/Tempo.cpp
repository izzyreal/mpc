#include "file/mid/event/meta/Tempo.hpp"

#include "file/mid/event/MidiEvent.hpp"
#include "file/mid/event/meta/GenericMetaEvent.hpp"
#include "file/mid/event/meta/MetaEventData.hpp"
#include "file/mid/event/meta/MetaEvent.hpp"
#include "file/mid/util/MidiUtil.hpp"
#include "file/mid/util/VariableLengthInt.hpp"

using namespace mpc::file::mid::event::meta;

Tempo::Tempo() : Tempo(0, 0, DEFAULT_MPQN) {}

Tempo::Tempo(int tick, int delta, int mpqn) : MetaEvent(tick, delta, TEMPO)
{
    setMpqn(mpqn);
    mLength = util::VariableLengthInt(3);
}

const int Tempo::DEFAULT_MPQN;

float Tempo::getBpm() const
{
    return mBPM;
}

void Tempo::setMpqn(int m)
{
    mMPQN = m;
    mBPM = 6.0E7f / mMPQN;
}

void Tempo::setBpm(float b)
{
    mBPM = b;
    mMPQN = static_cast<int>(60000000 / mBPM);
}

int Tempo::getEventSize()
{
    return 6;
}

void Tempo::writeToOutputStream(std::ostream &out)
{
    MetaEvent::writeToOutputStream(out);
    out << (char)3;
    auto mpqn = util::MidiUtil::intToBytes(mMPQN, 3);
    out.write(&mpqn[0], mpqn.size());
}

void Tempo::writeToOutputStream(std::ostream &out, bool writeType)
{
    MetaEvent::writeToOutputStream(out, writeType);
}

std::shared_ptr<MetaEvent> Tempo::parseTempo(int tick, int delta,
                                             MetaEventData *info)
{
    if (info->length.getValue() != 3)
    {
        return std::make_shared<GenericMetaEvent>(tick, delta, info);
    }
    auto mpqn = util::MidiUtil::bytesToInt(info->data, 0, 3);
    return std::make_shared<Tempo>(tick, delta, mpqn);
}

int Tempo::compareTo(MidiEvent *other)
{
    if (mTick != other->getTick())
    {
        return mTick < other->getTick() ? -1 : 1;
    }
    if (mDelta.getValue() != other->getDelta())
    {
        return mDelta.getValue() < other->getDelta() ? 1 : -1;
    }
    if (dynamic_cast<Tempo *>(other) == nullptr)
    {
        return 1;
    }
    auto o = dynamic_cast<Tempo *>(other);
    if (mMPQN != o->mMPQN)
    {
        return mMPQN < o->mMPQN ? -1 : 1;
    }
    return 0;
}
