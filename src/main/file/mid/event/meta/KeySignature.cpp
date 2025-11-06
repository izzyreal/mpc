#include "file/mid/event/meta/KeySignature.hpp"

#include "file/mid/event/MidiEvent.hpp"
#include "file/mid/event/meta/GenericMetaEvent.hpp"
#include "file/mid/event/meta/MetaEventData.hpp"
#include "file/mid/event/meta/MetaEvent.hpp"
#include "file/mid/util/VariableLengthInt.hpp"

using namespace mpc::file::mid::event::meta;

KeySignature::KeySignature(int tick, int delta, int key, int scale)
    : MetaEvent(tick, delta, MetaEvent::KEY_SIGNATURE)
{
    setKey(key);
    mScale = scale;
    mLength = mpc::file::mid::util::VariableLengthInt(2);
}

const int KeySignature::SCALE_MAJOR;
const int KeySignature::SCALE_MINOR;

void KeySignature::setKey(int key)
{
    mKey = static_cast<unsigned char>(key);
    if (mKey < -7)
    {
        mKey = -7;
    }
    else if (mKey > 7)
    {
        mKey = 7;
    }
}

int KeySignature::getKey() const
{
    return mKey;
}

void KeySignature::setScale(int scale)
{
    mScale = scale;
}

int KeySignature::getScale() const
{
    return mScale;
}

int KeySignature::getEventSize()
{
    return 5;
}

void KeySignature::writeToOutputStream(std::ostream &out)
{
    MetaEvent::writeToOutputStream(out);
    out << (char)0x02;
    out << (char)mKey;
    out << (char)mScale;
}

void KeySignature::writeToOutputStream(std::ostream &out, bool writeType)
{
    MetaEvent::writeToOutputStream(out, writeType);
}

std::shared_ptr<MetaEvent> KeySignature::parseKeySignature(int tick, int delta,
                                                           MetaEventData *info)
{
    if (info->length.getValue() != 2)
    {
        return std::make_shared<GenericMetaEvent>(tick, delta, info);
    }
    int key = info->data[0];
    int scale = info->data[1];
    return std::make_shared<KeySignature>(tick, delta, key, scale);
}

int KeySignature::compareTo(mpc::file::mid::event::MidiEvent *other) const
{
    if (mTick != other->getTick())
    {
        return mTick < other->getTick() ? -1 : 1;
    }

    if (mDelta.getValue() != other->getDelta())
    {
        return mDelta.getValue() < other->getDelta() ? 1 : -1;
    }

    if (dynamic_cast<KeySignature *>(other) == nullptr)
    {
        return 1;
    }

    auto o = dynamic_cast<KeySignature *>(other);

    if (mKey != o->mKey)
    {
        return mKey < o->mKey ? -1 : 1;
    }

    if (mScale != o->mScale)
    {
        return mKey < o->mScale ? -1 : 1;
    }

    return 0;
}
