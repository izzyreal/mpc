#pragma once

#include "Observer.hpp"

#include <memory>
#include <optional>
#include <unordered_map>
#include <atomic>
#include <cstdint>

namespace mpc
{
    class Mpc;
}

namespace mpc::sequencer
{
    class Track;
    class Event;
    class NoteOnEvent;
    class NoteOffEvent;
    class NoteOnEventPlayOnly;
} // namespace mpc::sequencer

namespace mpc::audiomidi
{
    class EventHandler final : public Observable
    {
    public:
        EventHandler(mpc::Mpc &mpc);

    private:
        using Track = mpc::sequencer::Track;
        using Event = mpc::sequencer::Event;
        using NoteOnEvent = mpc::sequencer::NoteOnEvent;
        using NoteOffEvent = mpc::sequencer::NoteOffEvent;
        using NoteOnEventPlayOnly = mpc::sequencer::NoteOnEventPlayOnly;

        std::atomic<uint64_t> noteEventId = 1;

    public:
        void handleFinalizedEvent(const std::shared_ptr<Event>, Track *const);

        // Handles physical pad presses.
        //
        // When triggered while in one of the sampler screens, drumIndex should
        // be derived from the drum that is selected in the DRUM select screen.
        // Here, we always have a drumIndex.
        //
        // In any of the other screens, drumIndex should be derived from the
        // active track. In this case, a std::nullopt can be provided to
        // indicate the track is not a DRUM track.
        //
        // While in one of the sampler screens, trackIndex and trackDevice must
        // be std::nullopt, because we only have a drum to work with, and no
        // track. No MIDI output events will be generated for pad presses in
        // this case. (We may need to check if this is what the MPC2000XL does.
        // If it does send MIDI output, the question is which device/channel it
        // uses where this information comes from.)
        //
        // While in any of the other screens, trackIndex and trackDevice must be
        // derived from the active track.
        void
        handleUnfinalizedNoteOn(const std::shared_ptr<NoteOnEvent>, Track *,
                                const std::optional<int> trackDevice,
                                const std::optional<int> trackVelocityRatio,
                                const std::optional<int> drumIndex);

        // Handles physical pad releases.
        //
        // When triggered while in one of the sampler screens, drumIndex should
        // be derived from the drum that is selected in the DRUM select screen.
        // Here, we always have a drumIndex.
        //
        // In any of the other screens, drumIndex should be derived from the
        // active track. In this case, a std::nullopt can be provided to
        // indicate the track is not a DRUM track.
        //
        // While in one of the sampler screens, trackIndex and trackDevice must
        // be std::nullopt, because we only have a drum to work with, and no
        // track. No MIDI output events will be generated for pad releases in
        // this case. (We may need to check if this is what the MPC2000XL does.
        // If it does send MIDI output, the question is which device/channel it
        // uses where this information comes from.)
        //
        // While in any of the other screens, trackIndex and trackDevice must be
        // derived from the active track.
        void
        handleNoteOffFromUnfinalizedNoteOn(const std::shared_ptr<NoteOffEvent>,
                                           Track *,
                                           const std::optional<int> trackDevice,
                                           const std::optional<int> drumIndex);

        void
        handleNoteOffFromFinalizedNoteOn(const std::shared_ptr<NoteOffEvent>);

        /**
         * Right now we only clear the cache when mpc::Mpc::panic() is invoked.
         * We may want to clear it every time the sequencer stops, but currently
         * I'm not sure about the exact behaviour with regard to active MIDI
         * note events when the sequencer is stopped -- is their note-off still
         * processed at a time that is scheduled when the associated note-on is
         * processed? Are we emitting note-offs for all active MIDI note-on
         * events when the sequencer stops?
         */
        void clearTransposeCache()
        {
            transposeCache.clear();
        }

    private:
        // trackVelocityRatio is std::nullopt for note off events, else must be
        // non-null
        void
        handleNoteEventMidiOut(const std::shared_ptr<Event>, Track *,
                               const int trackDevice,
                               const std::optional<int> trackVelocityRatio);

        /**
         * The MPC2000XL supports realtime, non-destructive transposition of
         * MIDI out events. The manual states on p. 74 that drum tracks are not
         * transposed. We also know, however, that a drum track that has a MIDI
         * device configured, will trigger both sample playback as well as MIDI
         * out events. At the moment I don't know if those MIDI events should
         * also be transposed. I'll have to investigate when I'm close to a real
         * MPC2000XL again. For now, we will do exactly what the manual states,
         * and only transpose pure MIDI tracks.
         *
         * To avoid allocation on the audio thread, we invoke `reserve(size_t)`
         * against the transpose cache in the constructor of EventHandler. We
         * must pass a number that represents the expected maximum number of
         * simultaneously active MIDI note events.
         *
         * MIDI has no spec that helps us decide what that number is, nor am I
         * aware of the internals of the MPC2000XL in this area. The manual
         * states that a sequence may contain 50,000 events, and we can play
         * back 2 sequences simultaneously, but realistically it's rare that
         * someone wants to have 100,000 simultaneous MIDI note events. So we'll
         * use a nice spacious number based on intuition, which is 512.
         */
        const size_t TRANSPOSE_CACHE_CAPACITY = 512;
        std::unordered_map<std::shared_ptr<NoteOffEvent>, int> transposeCache;

        mpc::Mpc &mpc;
    };
} // namespace mpc::audiomidi
