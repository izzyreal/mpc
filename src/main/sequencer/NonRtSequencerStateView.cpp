#include "sequencer/NonRtSequencerStateView.hpp"

#include "sequencer/NonRtSequencerState.hpp"

using namespace mpc::sequencer;

NonRtSequenceStateView::NonRtSequenceStateView(
    const NonRtSequenceState &s) noexcept
: state(s)
{
}

std::shared_ptr<NonRtTrackStateView>
NonRtSequenceStateView::getTrack(const int trackIndex) const
{
    return std::make_shared<NonRtTrackStateView>(state.tracks[trackIndex]);
}

NonRtSequencerStateView::NonRtSequencerStateView(
    const std::shared_ptr<const NonRtSequencerState> &s) noexcept
    : state(s)
{
}

PlaybackState NonRtSequencerStateView::getPlaybackState() const
{
    return PlaybackState();
}

std::shared_ptr<NonRtSequenceStateView>
NonRtSequencerStateView::getNonRtSequenceState(const SequenceIndex i) const
{
    return std::make_shared<NonRtSequenceStateView>(state->sequences[i]);
}

std::shared_ptr<NonRtTrackStateView>
NonRtSequencerStateView::getNonRtTrackState(const SequenceIndex sequenceIndex,
                                            const TrackIndex trackIndex) const
{
    return std::make_shared<NonRtTrackStateView>(state->sequences[sequenceIndex].tracks[trackIndex]);
}

NonRtTrackStateView::NonRtTrackStateView(const NonRtTrackState &s) noexcept
: state(s)
{

}

std::optional<EventState>
NonRtTrackStateView::findNoteEvent(const int tick, const NoteNumber note) const
{
    for (int i = 0; i < state.events.size(); ++i)
    {
        if (const auto &e = state.events[i]; e.type == EventType::NoteOn &&
                                              e.tick == tick &&
                                              e.noteNumber == note)
        {
            return e;
        }
    }

    return std::nullopt;
}

EventState NonRtTrackStateView::getEventByIndex(const EventIndex idx) const
{
    if (idx >= 0 && idx < state.events.size())
    {
        return state.events[idx];
    }
    return {};
}

std::vector<EventState>
NonRtTrackStateView::getEventRange(const int startTick, const int endTick) const
{
    std::vector<EventState> result;

    for (int i = 0; i < state.events.size(); ++i)
    {
        auto &e = state.events[i];
        if (e.tick > endTick)
        {
            break;
        }
        if (e.tick >= startTick && e.tick <= endTick)
        {
            result.push_back(e);
        }
    }

    return result;
}

bool NonRtTrackStateView::isEventsEmpty() const
{
    return state.events.empty();
}

int NonRtTrackStateView::getEventCount() const
{
    return state.events.size();
}

std::vector<EventState> NonRtTrackStateView::getNoteEvents() const
{
    std::vector<EventState> result;

    for (int i = 0; i < state.events.size(); ++i)
    {
        if (auto &e = state.events[i]; e.type == EventType::NoteOn)
        {
            result.push_back(e);
        }
    }

    return result;
}

EventState NonRtTrackStateView::findRecordingNoteOnByNoteNumber(
    const NoteNumber noteNumber) const
{
    for (auto &e : getNoteEvents())
    {
        if (e.noteNumber == noteNumber && e.beingRecorded)
        {
            return e;
        }
    }
    return {};
}

EventState NonRtTrackStateView::findRecordingNoteOnByNoteEventId(
    const NoteEventId id) const
{
    for (const auto &e : getNoteEvents())
    {
        if (e.noteEventId == id && e.beingRecorded)
        {
            return e;
        }
    }
    return {};
}

EventState NonRtTrackStateView::getEventById(const EventId eventId) const
{
    for (auto &e : state.events)
    {
        if (e.eventId == eventId)
        {
            return e;
        }
    }

    return {};
}
