#include "ConvertSongToSeqScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "engine/EngineHost.hpp"

#include "sequencer/EventRef.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

ConvertSongToSeqScreen::ConvertSongToSeqScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "convert-song-to-seq", layerIndex)
{
    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getSelectedSongIndex();
                        },
                        [&](auto)
                        {
                            displayFromSong();
                        }});
}

void ConvertSongToSeqScreen::open()
{
    const auto lockedSequencer = sequencer.lock();

    for (uint8_t idx = 0; idx < Mpc2000XlSpecs::SEQUENCE_COUNT; ++idx)
    {
        if (!lockedSequencer->getSequence(idx)->isUsed())
        {
            toSequenceIndex = SequenceIndex(idx);
            break;
        }
    }

    displayFromSong();
    displayToSequence();
    displayTrackStatus();
}

void ConvertSongToSeqScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SongScreen);
            break;
        case 4:
            convertSongToSeq();
            openScreenById(ScreenId::SongScreen);
            break;
        default:;
    }
}

void ConvertSongToSeqScreen::turnWheel(const int i)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "fromsong")
    {
        const auto lockedSequencer = sequencer.lock();
        lockedSequencer->setSelectedSongIndex(
            lockedSequencer->getSelectedSongIndex() + i);
    }
    else if (focusedFieldName == "tosequence")
    {
        setToSequenceIndex(toSequenceIndex + i);
    }
    else if (focusedFieldName == "trackstatus")
    {
        setTrackStatus(trackStatus + i);
    }
}

void ConvertSongToSeqScreen::setToSequenceIndex(
    const SequenceIndex toSequenceIndexToUse)
{
    toSequenceIndex =
        std::clamp(toSequenceIndexToUse, MinSequenceIndex, MaxSequenceIndex);

    displayToSequence();
}

void ConvertSongToSeqScreen::setTrackStatus(const int8_t newValue)
{
    const auto clampedNewValue = std::clamp<int8_t>(newValue, 0, 2);
    trackStatus = clampedNewValue;
    displayTrackStatus();
}

void ConvertSongToSeqScreen::displayFromSong() const
{
    const auto lockedSequencer = sequencer.lock();
    const auto songIndex = lockedSequencer->getSelectedSongIndex();
    const auto song = lockedSequencer->getSelectedSong();
    const auto songIndexString =
        StrUtil::padLeft(std::to_string(songIndex + 1), "0", 2);
    const auto songName = song->getName();
    findField("fromsong")->setText(songIndexString + "-" + songName);
}

void ConvertSongToSeqScreen::displayToSequence() const
{
    const auto lockedSequencer = sequencer.lock();
    const auto toSequence = lockedSequencer->getSequence(toSequenceIndex);
    const auto sequenceIndexString =
        StrUtil::padLeft(std::to_string(toSequenceIndex + 1), "0", 2);
    const auto sequenceName = toSequence->getName();
    findField("tosequence")->setText(sequenceIndexString + "-" + sequenceName);
}

void ConvertSongToSeqScreen::displayTrackStatus() const
{
    findField("trackstatus")->setText(trackStatusNames[trackStatus]);
}

void eraseEventsFromOffTracks(
    const int firstBarToRemove, const int firstBarToKeep,
    const std::shared_ptr<Sequence> &sourceSequence,
    const std::shared_ptr<Sequence> &destinationSequence)
{
    const auto startTick =
        destinationSequence->getFirstTickOfBar(firstBarToRemove);
    const auto endTick = destinationSequence->getFirstTickOfBar(firstBarToKeep);

    for (const auto &track : destinationSequence->getTracks())
    {
        if (sourceSequence->getTrack(track->getIndex())->isOn())
        {
            continue;
        }

        for (const auto &e : track->getEventHandles())
        {
            if (e->tick >= startTick && e->tick < endTick)
            {
                track->removeEvent(e);
            }
        }
    }
}

void ConvertSongToSeqScreen::convertSongToSeq() const
{
    utils::Task audioTask(
        [this]
        {
            const auto lockedSequencer = sequencer.lock();
            const auto stateManager = lockedSequencer->getStateManager();

            const auto song = lockedSequencer->getSelectedSong();

            if (!song->isUsed())
            {
                return;
            }

            const auto destinationSequence =
                lockedSequencer->getSequence(toSequenceIndex);

            destinationSequence->init(0);

            stateManager->drainQueue();

            destinationSequence->getTempoChangeTrack()->removeEvents();

            stateManager->drainQueue();

            destinationSequence->setName(song->getName());

            for (int stepIndex = 0; stepIndex < song->getStepCount();
                 ++stepIndex)
            {
                const auto step = song->getStep(SongStepIndex(stepIndex));
                const auto sourceSequenceIndex = step.sequenceIndex;
                const auto sourceSequence =
                    lockedSequencer->getSequence(sourceSequenceIndex);
                const auto
                    destinationSequenceLastBarIndexBeforeProcessingCurrentStep =
                        destinationSequence->getLastBarIndex();

                if (!sourceSequence->isUsed())
                {
                    break;
                }

                if (stepIndex == 0)
                {
                    destinationSequence->setInitialTempo(
                        sourceSequence->getInitialTempo());
                    stateManager->drainQueue();
                }

                const auto sourceSequencelastBarIndex =
                    sourceSequence->getLastBarIndex();

                const auto repetitionCount = step.repetitionCount;

                if (trackStatus == 0 || trackStatus == 1)
                {
                    SeqUtil::copyBars(
                        mpc, sourceSequenceIndex, toSequenceIndex, 0,
                        sourceSequencelastBarIndex, repetitionCount,
                        destinationSequenceLastBarIndexBeforeProcessingCurrentStep);

                    stateManager->drainQueue();

                    if (trackStatus == 1)
                    {
                        const auto firstBarIndexToRemove =
                            destinationSequenceLastBarIndexBeforeProcessingCurrentStep;
                        const auto addedBarCount =
                            sourceSequence->getBarCount() * repetitionCount;
                        const auto firstBarIndexToKeep =
                            firstBarIndexToRemove + addedBarCount;

                        eraseEventsFromOffTracks(
                            firstBarIndexToRemove, firstBarIndexToKeep,
                            sourceSequence, destinationSequence);
                        stateManager->drainQueue();
                    }
                }
                else /* if (trackStatus == 3) */
                {
                    { // Append bars with correct time signatures to destination
                      // sequence
                        const auto barCountToAdd =
                            sourceSequence->getBarCount() * repetitionCount;

                        destinationSequence->setLastBarIndex(
                            destinationSequenceLastBarIndexBeforeProcessingCurrentStep +
                            barCountToAdd);

                        stateManager->drainQueue();

                        auto destinationBarIndex =
                            destinationSequenceLastBarIndexBeforeProcessingCurrentStep;

                        for (int repetition = 0; repetition < repetitionCount;
                             repetition++)
                        {
                            for (int barCounter = 0;
                                 barCounter < sourceSequence->getBarCount();
                                 barCounter++)
                            {
                                const auto numerator =
                                    sourceSequence->getNumerator(barCounter);

                                const auto denominator =
                                    sourceSequence->getDenominator(barCounter);

                                destinationSequence->setTimeSignature(
                                    destinationBarIndex, numerator,
                                    denominator);

                                stateManager->drainQueue();

                                destinationBarIndex++;
                            }
                        }
                    }

                    for (const auto &sourceTrack : sourceSequence->getTracks())
                    {
                        const auto midiChannel =
                            sourceTrack->getDeviceIndex() - 1;

                        std::shared_ptr<Track> destinationTrack;

                        if (midiChannel >= 0)
                        {
                            // copy to track indexes 0 - 31
                            destinationTrack =
                                destinationSequence->getTrack(midiChannel);
                        }
                        else
                        {
                            // copy to destination track indexes 32 - 35 as per
                            // source track drum bus
                            if (sourceTrack->getBusType() == BusType::MIDI &&
                                sourceTrack->getIndex() !=
                                    TempoChangeTrackIndex)
                            {
                                // The source track has neither a MIDI out
                                // device, nor a DRUM bus. Nothing to copy to
                                // the destination sequence in this case.
                                continue;
                            }

                            const auto drumBusIndex = drumBusTypeToDrumIndex(
                                sourceTrack->getBusType());

                            destinationTrack = destinationSequence->getTrack(
                                32 + drumBusIndex);
                        }

                        const auto destinationFirstBarIndex =
                            destinationSequenceLastBarIndexBeforeProcessingCurrentStep;

                        const auto destinationFirstBarStartTick =
                            destinationSequence->getFirstTickOfBar(
                                destinationFirstBarIndex);

                        for (const auto &sourceEvent : sourceTrack->getEvents())
                        {
                            const auto destinationTick =
                                destinationFirstBarStartTick +
                                sourceEvent->getTick();

                            EventData eventState = *sourceEvent->handle;
                            eventState.tick = destinationTick;

                            if (sourceTrack->getSequenceIndex() ==
                                TempoChangeTrackIndex)
                            {
                                destinationSequence
                                    ->getTrack(TempoChangeTrackIndex)
                                    ->acquireAndInsertEvent(eventState);
                            }
                            else
                            {
                                destinationTrack->acquireAndInsertEvent(
                                    eventState);
                            }
                            stateManager->drainQueue();
                        }
                    }
                }
            }

            if (destinationSequence->getLastBarIndex() == 0)
            {
                destinationSequence->setUsed(false);
                stateManager->drainQueue();
                return;
            }

            destinationSequence->setLastBarIndex(
                destinationSequence->getLastBarIndex() - 1);

            stateManager->drainQueue();

            if (trackStatus == 0 || trackStatus == 1)
            {
                const auto referenceStep = song->getStep(MinSongStepIndex);
                const auto referenceSequence =
                    lockedSequencer->getSequence(referenceStep.sequenceIndex);

                for (int trackIndex = 0;
                     trackIndex < Mpc2000XlSpecs::TRACK_COUNT; ++trackIndex)
                {
                    auto referenceTrack =
                        referenceSequence->getTrack(trackIndex);

                    auto destTrack = destinationSequence->getTrack(trackIndex);

                    lockedSequencer->copyTrackParameters(referenceTrack,
                                                         destTrack);

                    stateManager->drainQueue();

                    if (trackStatus == 1)
                    {
                        destTrack->setOn(true);
                    }
                }
            }

            stateManager->drainQueue();
        });

    mpc.getEngineHost()->postToAudioThread(std::move(audioTask));
}
