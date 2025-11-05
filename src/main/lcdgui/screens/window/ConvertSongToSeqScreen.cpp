#include "ConvertSongToSeqScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "lcdgui/screens/SongScreen.hpp"

#include "sequencer/Event.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

ConvertSongToSeqScreen::ConvertSongToSeqScreen(mpc::Mpc &mpc,
                                               const int layerIndex)
    : ScreenComponent(mpc, "convert-song-to-seq", layerIndex)
{
}

void ConvertSongToSeqScreen::open()
{
    for (uint8_t i = 0; i < 99; i++)
    {
        toSequenceIndex = i;

        if (!sequencer->getSequence(i)->isUsed())
        {
            break;
        }
    }

    displayFromSong();
    displayToSequence();
    displayTrackStatus();
}

void ConvertSongToSeqScreen::function(int i)
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
    }
}

void ConvertSongToSeqScreen::turnWheel(int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "fromsong")
    {
        const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
        setFromSong(songScreen->activeSongIndex + i);
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

void ConvertSongToSeqScreen::setFromSong(int8_t newValue)
{
    const auto clampedNewValue = std::clamp<int8_t>(newValue, 0, 19);
    const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    songScreen->setActiveSongIndex(clampedNewValue);
    displayFromSong();
}

void ConvertSongToSeqScreen::setToSequenceIndex(int8_t newValue)
{
    const auto clampedNewValue = std::clamp<int8_t>(newValue, 0, 98);
    toSequenceIndex = clampedNewValue;
    displayToSequence();
}

void ConvertSongToSeqScreen::setTrackStatus(int8_t newValue)
{
    const auto clampedNewValue = std::clamp<int8_t>(newValue, 0, 2);
    trackStatus = clampedNewValue;
    displayTrackStatus();
}

void ConvertSongToSeqScreen::displayFromSong()
{
    const auto activeSongIndex =
        mpc.screens->get<ScreenId::SongScreen>()->getActiveSongIndex();
    const auto activeSong = sequencer->getSong(activeSongIndex);
    const auto songIndexString =
        StrUtil::padLeft(std::to_string(activeSongIndex + 1), "0", 2);
    const auto songName = activeSong->getName();
    findField("fromsong")->setText(songIndexString + "-" + songName);
}

void ConvertSongToSeqScreen::displayToSequence()
{
    const auto activeSequence = sequencer->getSequence(toSequenceIndex);
    const auto sequenceIndexString =
        StrUtil::padLeft(std::to_string(toSequenceIndex + 1), "0", 2);
    const auto sequenceName = activeSequence->getName();
    findField("tosequence")->setText(sequenceIndexString + "-" + sequenceName);
}

void ConvertSongToSeqScreen::displayTrackStatus()
{
    findField("trackstatus")->setText(trackStatusNames[trackStatus]);
}

void eraseOffTracks(const int firstBarToRemove, const int firstBarToKeep,
                    const std::shared_ptr<Sequence> &sourceSequence,
                    const std::shared_ptr<Sequence> &destinationSequence)
{
    const auto startTick =
        destinationSequence->getFirstTickOfBar(firstBarToRemove);
    const auto endTick = destinationSequence->getFirstTickOfBar(firstBarToKeep);

    for (auto &track : destinationSequence->getTracks())
    {
        if (sourceSequence->getTrack(track->getIndex())->isOn())
        {
            continue;
        }

        for (int i = track->getEvents().size() - 1; i >= 0; i--)
        {
            const auto event = track->getEvent(i);

            if (event->getTick() >= startTick && event->getTick() < endTick)
            {
                track->removeEvent(event);
            }
        }
    }
}

void ConvertSongToSeqScreen::convertSongToSeq()
{
    const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
    const auto song = sequencer->getSong(songScreen->activeSongIndex);

    if (!song->isUsed())
    {
        return;
    }

    const auto destinationSequence = sequencer->getSequence(toSequenceIndex);

    destinationSequence->init(0);
    destinationSequence->setName(song->getName());

    for (int stepIndex = 0; stepIndex < song->getStepCount(); stepIndex++)
    {
        const auto step = song->getStep(stepIndex).lock();
        const auto sourceSequenceIndex = step->getSequence();
        const auto sourceSequence = sequencer->getSequence(sourceSequenceIndex);
        const auto destinationSequenceLastBarIndexBeforeProcessingCurrentStep =
            destinationSequence->getLastBarIndex();

        if (!sourceSequence->isUsed())
        {
            break;
        }

        if (stepIndex == 0)
        {
            destinationSequence->setInitialTempo(
                sourceSequence->getInitialTempo());
        }

        const auto sourceSequencelastBarIndex =
            sourceSequence->getLastBarIndex();
        const auto repeatCount = step->getRepeats();

        if (trackStatus == 0 || trackStatus == 1)
        {
            SeqUtil::copyBars(
                mpc, sourceSequenceIndex, toSequenceIndex, 0,
                sourceSequencelastBarIndex, repeatCount,
                destinationSequenceLastBarIndexBeforeProcessingCurrentStep);

            if (trackStatus == 1)
            {
                const auto firstBarIndexToRemove =
                    destinationSequenceLastBarIndexBeforeProcessingCurrentStep;
                const auto addedBarCount =
                    sourceSequence->getBarCount() * repeatCount;
                const auto firstBarIndexToKeep =
                    firstBarIndexToRemove + addedBarCount;

                eraseOffTracks(firstBarIndexToRemove, firstBarIndexToKeep,
                               sourceSequence, destinationSequence);
            }
        }
        else /* if (trackStatus == 3) */
        {
            { // Append bars with correct time signatures to destination
              // sequence
                const auto barCountToAdd =
                    sourceSequence->getBarCount() * repeatCount;
                destinationSequence->setLastBarIndex(
                    destinationSequenceLastBarIndexBeforeProcessingCurrentStep +
                    barCountToAdd);
                auto destinationBarIndex =
                    destinationSequenceLastBarIndexBeforeProcessingCurrentStep;

                for (int repetition = 0; repetition < repeatCount; repetition++)
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
                            destinationBarIndex, numerator, denominator);

                        destinationBarIndex++;
                    }
                }
            }

            for (auto &sourceTrack : sourceSequence->getTracks())
            {
                const auto midiChannel = sourceTrack->getDeviceIndex() - 1;

                std::shared_ptr<Track> destinationTrack;

                if (midiChannel >= 0)
                {
                    // copy to track indexes 0 - 31
                    destinationTrack =
                        destinationSequence->getTrack(midiChannel);
                }
                else
                {
                    // copy to destination track indexes 32 - 35 as per source
                    // track drum bus
                    const auto drumBusIndex = sourceTrack->getBus() - 1;

                    if (drumBusIndex < 0)
                    {
                        // The source track has neither a MIDI out device, nor a
                        // DRUM bus. Nothing to copy to the destination sequence
                        // in this case.
                        continue;
                    }

                    destinationTrack =
                        destinationSequence->getTrack(32 + drumBusIndex);
                }

                const auto destinationFirstBarIndex =
                    destinationSequenceLastBarIndexBeforeProcessingCurrentStep;
                const auto destinationFirstBarStartTick =
                    destinationSequence->getFirstTickOfBar(
                        destinationFirstBarIndex);

                for (auto &sourceEvent : sourceTrack->getEvents())
                {
                    const auto destinationTick =
                        destinationFirstBarStartTick + sourceEvent->getTick();
                    destinationTrack->cloneEventIntoTrack(
                        sourceEvent, destinationTick, true);
                }
            }
        }
    }

    if (destinationSequence->getLastBarIndex() == 0)
    {
        destinationSequence->setUsed(false);
        return;
    }

    destinationSequence->setLastBarIndex(
        destinationSequence->getLastBarIndex() - 1);

    if (trackStatus == 0 || trackStatus == 1)
    {
        const auto referenceStep = song->getStep(0).lock();
        const auto referenceSequence =
            sequencer->getSequence(referenceStep->getSequence());

        for (int trackIndex = 0; trackIndex < 64; trackIndex++)
        {
            auto referenceTrack = referenceSequence->getTrack(trackIndex);
            auto destTrack = destinationSequence->getTrack(trackIndex);
            sequencer->copyTrackParameters(referenceTrack, destTrack);

            if (trackStatus == 1)
            {
                destTrack->setOn(true);
            }
        }
    }
}
