#pragma once

#include "sequencer/TrackState.hpp"
#include "sequencer/TimeSignature.hpp"
#include "sequencer/TrackMessage.hpp"

#include "utils/SmallFn.hpp"

#include <variant>
#include <vector>
#include <unordered_map>

namespace mpc::sequencer
{
    using SequenceTrackEventsSnapshot = std::unordered_map<uint8_t, std::vector<EventData>>;
    using SequenceTrackStatesSnapshot = std::array<TrackState, Mpc2000XlSpecs::TRACK_COUNT>;

    struct SetTimeSignature
    {
        SequenceIndex sequenceIndex;
        BarIndex barIndex;
        TimeSignature timeSignature;
    };

    // This message only deals with non-meta track events, i.e. tracks
    // 0 to 63. Tempo change events need to be dealt with separately.
    // The handler also assumes each track has been cleared of any unwanted
    // events, so it performs no cleanup or deletion whatsoever.
    // The events in the track snapshots are assumed to be unacquired. The
    // handler will acquire memory slots for the events from the event pool.
    struct UpdateSequenceEvents
    {
        SequenceIndex sequenceIndex;
        SequenceTrackEventsSnapshot *trackSnapshots;
    };

    struct UpdateSequenceTracks
    {
        SequenceIndex sequenceIndex;
        SequenceTrackStatesSnapshot *trackStates;
    };

    struct SetLastBarIndex
    {
        SequenceIndex sequenceIndex;
        BarIndex barIndex;
    };

    struct InsertBars
    {
        using Callback = utils::SmallFn<64, void(BarIndex)>;
        SequenceIndex sequenceIndex;
        int barCount;
        BarIndex afterBar;
    };

    struct DeleteBars
    {
        SequenceIndex sequenceIndex;
        BarIndex firstBarIndex; // inclusive
        BarIndex lastBarIndex;  // inclusive
    };

    struct SetInitialTempo
    {
        SequenceIndex sequenceIndex;
        double initialTempo;
    };

    struct SetLoopEnabled
    {
        SequenceIndex sequenceIndex;
        bool loopEnabled;
    };

    struct SetSequenceUsed
    {
        SequenceIndex sequenceIndex;
        bool used;
    };

    struct SetTempoChangeEnabled
    {
        SequenceIndex sequenceIndex;
        bool tempoChangeEnabled;
    };

    struct SetFirstLoopBarIndex
    {
        SequenceIndex sequenceIndex;
        BarIndex barIndex;
    };

    struct SetLastLoopBarIndex
    {
        SequenceIndex sequenceIndex;
        BarIndex barIndex;
    };

    struct SyncTrackEventIndices
    {
        SequenceIndex sequenceIndex;
    };

    struct MoveTrack
    {
        SequenceIndex sequenceIndex;
        TrackIndex source;
        TrackIndex destination;
    };

    struct DeleteTrack
    {
        SequenceIndex sequenceIndex;
        TrackIndex trackIndex;
        int8_t deviceIndex;
        uint8_t programChange;
        BusType busType;
        uint8_t velocityRatio;
    };

    struct DeleteAllTracks
    {
        SequenceIndex sequenceIndex;
        int8_t deviceIndex;
        uint8_t programChange;
        BusType busType;
        uint8_t velocityRatio;
    };

    struct SetSequenceName
    {
        SequenceIndex sequenceIndex;
        char name[Mpc2000XlSpecs::MAX_SEQUENCE_NAME_LENGTH + 1];
    };

    using SequenceMessage =
        std::variant<TrackMessage, SetLastBarIndex, InsertBars, SetInitialTempo,
                     SetTimeSignature, SetLoopEnabled, SetSequenceUsed,
                     SetTempoChangeEnabled, SetFirstLoopBarIndex,
                     SetLastLoopBarIndex, SyncTrackEventIndices, MoveTrack,
                     DeleteTrack, DeleteAllTracks, DeleteBars,
                     UpdateSequenceEvents, UpdateSequenceTracks,
                     SetSequenceName>;
} // namespace mpc::sequencer
