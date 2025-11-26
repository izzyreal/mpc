#pragma once

#include "NonRtSequencerState.hpp"
#include "PlaybackState.hpp"
#include "sequencer/EventState.hpp"

#include <memory>
#include <vector>

namespace mpc::sequencer
{
    struct NonRtSequencerState;

    class NonRtTrackStateView
    {
    public:
        explicit NonRtTrackStateView(
            const NonRtTrackState &s) noexcept;

        std::optional<EventState> findNoteEvent(int tick,
                                                NoteNumber note) const;

        EventState getEventByIndex(EventIndex) const;

        std::vector<EventState> getEventRange(int startTick, int endTick) const;

        bool isEventsEmpty() const;

        int getEventCount() const;

        std::vector<EventState> getNoteEvents() const;

        std::vector<EventState> getEvents() const;

        EventState findRecordingNoteOnByNoteNumber(NoteNumber) const;

        EventState findRecordingNoteOnByNoteEventId(NoteEventId) const;

        EventState getEventById(EventId) const;

    private:
        const NonRtTrackState &state;
    };

    class NonRtSequenceStateView
    {
    public:
        explicit NonRtSequenceStateView(
            const NonRtSequenceState &s) noexcept;

        std::shared_ptr<NonRtTrackStateView> getTrack(int trackIndex) const;
        Tick getBarLength(int barIndex) const;
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> getBarLengths() const;
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> getTimeSignatures() const;

        TimeSignature getTimeSignature(int barIndex) const;

    private:
        const NonRtSequenceState &state;
    };

    class NonRtSequencerStateView
    {
    public:
        explicit NonRtSequencerStateView(
            const std::shared_ptr<const NonRtSequencerState> &s) noexcept;

        PlaybackState getPlaybackState() const;

        std::shared_ptr<NonRtSequenceStateView> getNonRtSequenceState(SequenceIndex) const;

        std::shared_ptr<NonRtTrackStateView> getNonRtTrackState(SequenceIndex, TrackIndex) const;

        SequenceIndex getSelectedSequenceIndex() const noexcept;

        double getPositionQuarterNotes() const;

        double getPlayStartPositionQuarterNotes() const;

        int64_t getPositionTicks() const;

        bool isSequencerRunning() const;

    private:
        const std::shared_ptr<const NonRtSequencerState> state;
    };
} // namespace mpc::sequencer
