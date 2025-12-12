#include "sequencer/SongStateHandler.hpp"

#include "sequencer/SequencerState.hpp"
#include "utils/VariantUtils.hpp"

using namespace mpc::sequencer;

SongStateHandler::SongStateHandler() {}

SongStateHandler::~SongStateHandler() {}

void SongStateHandler::applyMessage(SequencerState &state,
                                    std::vector<utils::SimpleAction> &actions,
                                    const SongMessage &msg)
{
    const auto visitor = Overload{
        [&](const SetSongStepSequenceIndex &m)
        {
            state.songs[m.songIndex].steps[m.stepIndex].sequenceIndex =
                m.sequenceIndex;
        },
        [&](const SetSongStepRepetitionCount &m)
        {
            state.songs[m.songIndex].steps[m.stepIndex].repetitionCount =
                m.count;
        },
        [&](const SetSongLoopEnabled &m)
        {
            state.songs[m.songIndex].loopEnabled = m.enabled;
        },
        [&](const SetSongFirstLoopStepIndex &m)
        {
            auto &song = state.songs[m.songIndex];
            song.firstLoopStepIndex = m.stepIndex;

            if (song.firstLoopStepIndex > song.lastLoopStepIndex)
            {
                song.lastLoopStepIndex = song.firstLoopStepIndex;
            }
        },
        [&](const SetSongLastLoopStepIndex &m)
        {
            auto &song = state.songs[m.songIndex];
            song.lastLoopStepIndex = m.stepIndex;

            if (song.lastLoopStepIndex < song.firstLoopStepIndex)
            {
                song.firstLoopStepIndex = song.lastLoopStepIndex;
            }
        },
        [&](const DeleteSongStep &m)
        {
            auto &song = state.songs[m.songIndex];
            const auto first = song.steps.begin() + m.stepIndex;
            const auto last = song.steps.end();
            std::rotate(first, first + 1, last);
            (last - 1)->sequenceIndex = NoSequenceIndex;

            const auto it =
                std::find_if(song.steps.begin(), song.steps.end(),
                             [&](const auto &s)
                             {
                                 return s.sequenceIndex == NoSequenceIndex;
                             });

            const int stepCount = it == song.steps.end()
                                      ? song.steps.size()
                                      : std::distance(song.steps.begin(), it);

            if (song.lastLoopStepIndex >= stepCount)
            {
                song.lastLoopStepIndex = SongStepIndex(stepCount - 1);
            }
        },
        [&](const InsertSongStep &m)
        {
            auto &song = state.songs[m.songIndex];
            const auto first = song.steps.begin() + m.stepIndex;
            const auto last = song.steps.end();
            std::rotate(first, last - 1, last);
            auto &dst = song.steps[m.stepIndex];
            dst.sequenceIndex = MinSequenceIndex;
            dst.repetitionCount = 1;
        },
        [&](const SetSongUsed &m)
        {
            auto &song = state.songs[m.songIndex];
            song.used = m.used;
        },
        [&](const DeleteSong &m)
        {
            auto &song = state.songs[m.songIndex];
            song.initializeDefaults();
        }};

    std::visit(visitor, msg);
}
