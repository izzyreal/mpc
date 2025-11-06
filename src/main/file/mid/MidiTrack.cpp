#include "file/mid/MidiTrack.hpp"

#include "file/mid/event/MidiEvent.hpp"
#include "file/mid/event/NoteOn.hpp"
#include "file/mid/event/meta/EndOfTrack.hpp"
#include "file/mid/event/meta/Tempo.hpp"
#include "file/mid/event/meta/TimeSignatureEvent.hpp"
#include "file/mid/util/MidiUtil.hpp"
#include "file/mid/util/VariableLengthInt.hpp"

#include <stdexcept>
#include <sstream>

#include <Logger.hpp>

using namespace mpc::file::mid;
using namespace mpc::file::mid::util;
using namespace mpc::file::mid::event;

MidiTrack::MidiTrack()
{
    mSize = 0;
    mSizeNeedsRecalculating = false;
    mClosed = false;
    mEndOfTrackDelta = 0;
}

MidiTrack::MidiTrack(const std::shared_ptr<std::istream> &stream)
{
    mSize = 0;
    mSizeNeedsRecalculating = false;
    mClosed = false;
    mEndOfTrackDelta = 0;
    std::vector<char> buffer(4);

    stream->read(&buffer[0], buffer.size());

    if (!MidiUtil::bytesEqual(buffer, IDENTIFIER, 0, 4))
    {
        std::string error = "Track identifier did not match MTrk!";
        throw std::invalid_argument(error);
    }

    stream->read(&buffer[0], buffer.size());

    mSize = mpc::file::mid::util::MidiUtil::bytesToInt(buffer, 0, 4);
    buffer.clear();
    buffer.resize(mSize);
    stream->read(&buffer[0], mSize);

    readTrackData(buffer);
}

const bool MidiTrack::VERBOSE;

std::vector<char> MidiTrack::IDENTIFIER{'M', 'T', 'r', 'k'};

MidiTrack *MidiTrack::createTempoTrack()
{
    auto track = new MidiTrack();
    track->insertEvent(std::make_shared<meta::TimeSignature>());
    track->insertEvent(std::make_shared<meta::Tempo>());
    return track;
}

void MidiTrack::readTrackData(std::vector<char> &data)
{
    std::stringstream in(std::string(data.begin(), data.end()));
    in.unsetf(std::ios_base::skipws);

    int totalTicks = 0;

    auto available = (int)in.rdbuf()->in_avail();

    while ((available = (int)in.rdbuf()->in_avail() > 0))
    {
        auto delta = mpc::file::mid::util::VariableLengthInt(in);
        int value = delta.getValue();
        totalTicks += value;

        auto event = MidiEvent::parseEvent(totalTicks, value, in);

        if (!event)
        {
            MLOG("MidiTrack readTrackData skipped an event!");
            continue;
        }

        if (std::dynamic_pointer_cast<meta::EndOfTrack>(event))
        {
            mEndOfTrackDelta = event->getDelta();
            break;
        }

        mEvents.emplace_back(event);
    }
}

std::vector<std::weak_ptr<MidiEvent>> MidiTrack::getEvents() const
{
    std::vector<std::weak_ptr<MidiEvent>> res;
    for (auto &e : mEvents)
    {
        res.push_back(e);
    }
    return res;
}

int MidiTrack::getEventCount() const
{
    return mEvents.size();
}

int MidiTrack::getSize()
{
    if (mSizeNeedsRecalculating)
    {
        recalculateSize();
    }
    return mSize;
}

int MidiTrack::getLengthInTicks()
{
    if (mEvents.size() == 0)
    {
        return 0;
    }
    auto E = *std::next(mEvents.begin(), (int)(mEvents.size()) - 1);
    return E->getTick();
}

int MidiTrack::getEndOfTrackDelta() const
{
    return mEndOfTrackDelta;
}

void MidiTrack::setEndOfTrackDelta(int delta)
{
    mEndOfTrackDelta = delta;
}

void MidiTrack::insertNote(int channel, int pitch, int velocity, int tick,
                           int duration)
{
    insertEvent(std::make_shared<NoteOn>(tick, channel, pitch, velocity));
    insertEvent(std::make_shared<NoteOn>(tick + duration, channel, pitch, 0));
}

void MidiTrack::insertEvent(const std::weak_ptr<event::MidiEvent> &newE)
{
    auto newEvent = newE.lock();
    if (!newEvent)
    {
        return;
    }
    if (mClosed)
    {
        // npc(::java::lang::System::err())->println(u"Error: Cannot add an
        // event to a closed track."_j);
        return;
    }
    std::shared_ptr<MidiEvent> prev;
    std::shared_ptr<MidiEvent> next;
    if (mEvents.size() > 0)
    {
        prev = mEvents.back();
    }

    /*
    for (auto& n : mEvents) {
        next = n;
        if (next->getTick() > newEvent->getTick()) {
            break;
        }
        prev = next;
        next.reset();
    }
    */

    mEvents.push_back(newEvent);
    mSizeNeedsRecalculating = true;

    if (prev)
    {
        newEvent->setDelta(newEvent->getTick() - prev->getTick());
    }
    else
    {
        newEvent->setDelta(newEvent->getTick());
    }
    if (next)
    {
        next->setDelta(next->getTick() - newEvent->getTick());
    }

    mSize += newEvent->getSize();

    if (std::dynamic_pointer_cast<meta::EndOfTrack>(newEvent))
    {
        if (next)
        {
            throw std::invalid_argument(
                "Attempting to insert EndOfTrack before an existing event. Use "
                "closeTrack() when finished with MidiTrack.");
        }
        mClosed = true;
    }
}

bool MidiTrack::removeEvent(mpc::file::mid::event::MidiEvent *e) const
{
    std::shared_ptr<MidiEvent> prev;
    std::shared_ptr<MidiEvent> curr;
    std::shared_ptr<MidiEvent> next;

    for (auto &n : mEvents)
    {
        next = n;
        if (e == curr.get())
        {
            break;
        }
        prev = curr;
        curr = next;
        next = nullptr;
    }
    if (!next)
    {
        // return mEvents.erase(curr);
        return false;
    }
    // if (!mEvents.erase(curr)) {
    //		return false;
    //}
    if (prev != nullptr)
    {
        next->setDelta(next->getTick() - prev->getTick());
    }
    else
    {
        next->setDelta(next->getTick());
    }
    return true;
}

void MidiTrack::closeTrack()
{
    int lastTick = 0;
    if (mEvents.size() > 0)
    {
        auto last = std::dynamic_pointer_cast<MidiEvent>(
            *std::next(mEvents.begin(), (int)(mEvents.size()) - 1));
        lastTick = last->getTick();
    }
    insertEvent(
        std::make_shared<meta::EndOfTrack>(lastTick + mEndOfTrackDelta, 0));
}

void MidiTrack::recalculateSize()
{
    mSize = 0;
    std::shared_ptr<MidiEvent> last;
    for (auto &e : mEvents)
    {
        mSize += e->getSize();
        if (last && !e->requiresStatusByte(last.get()))
        {
            mSize--;
        }
        last = e;
    }
    mSizeNeedsRecalculating = false;
}

void MidiTrack::writeToOutputStream(const std::shared_ptr<std::ostream> &stream)
{
    if (!mClosed)
    {
        closeTrack();
    }

    if (mSizeNeedsRecalculating)
    {
        recalculateSize();
    }

    stream->write(&IDENTIFIER[0], IDENTIFIER.size());

    auto trackSizeBuffer = mpc::file::mid::util::MidiUtil::intToBytes(mSize, 4);
    stream->write(&trackSizeBuffer[0], trackSizeBuffer.size());

    std::shared_ptr<MidiEvent> lastEvent;

    for (auto &event : mEvents)
    {
        event->writeToOutputStream(*stream.get(),
                                   event->requiresStatusByte(lastEvent.get()));
        lastEvent = event;
    }
}
