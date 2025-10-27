#include "midi/event/meta/TextualMetaEvent.hpp"

#include "midi/event/MidiEvent.hpp"
#include "midi/util/VariableLengthInt.hpp"

using namespace mpc::midi::event::meta;

TextualMetaEvent::TextualMetaEvent(int tick, int delta, int type,
                                   std::string text)
    : MetaEvent(tick, delta, type)
{
    setText(text);
    mLength = mpc::midi::util::VariableLengthInt(text.length());
}

void TextualMetaEvent::setText(std::string t)
{
    mText = t;
    mLength.setValue(t.length());
}

std::string TextualMetaEvent::getText()
{
    return mText;
}

int TextualMetaEvent::getEventSize()
{
    return 1 + 1 + mLength.getByteCount() + mLength.getValue();
}

void TextualMetaEvent::writeToOutputStream(std::ostream &out)
{
    MetaEvent::writeToOutputStream(out);
    auto length = mLength.getBytes();
    out.write(&length[0], length.size());
    out.write(&mText[0], mText.size());
}

void TextualMetaEvent::writeToOutputStream(std::ostream &out, bool writeType)
{
    MetaEvent::writeToOutputStream(out, writeType);
}

int TextualMetaEvent::compareTo(mpc::midi::event::MidiEvent *other)
{
    if (mTick != other->getTick())
    {
        return mTick < other->getTick() ? -1 : 1;
    }
    if (mDelta.getValue() != other->getDelta())
    {
        return mDelta.getValue() < other->getDelta() ? 1 : -1;
    }
    if (dynamic_cast<TextualMetaEvent *>(other) == nullptr)
    {
        return 1;
    }
    auto o = dynamic_cast<TextualMetaEvent *>(other);
    return mText.compare(o->mText);
}

std::string TextualMetaEvent::toString()
{
    return MetaEvent::toString() + ": " + mText;
}
