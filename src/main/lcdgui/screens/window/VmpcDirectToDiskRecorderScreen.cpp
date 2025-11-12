#include "VmpcDirectToDiskRecorderScreen.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "engine/EngineHost.hpp"
#include "audiomidi/DirectToDiskSettings.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/SeqUtil.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "lcdgui/Label.hpp"

#include "lcdgui/screens/SongScreen.hpp"

using namespace mpc;
using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace mpc::sequencer;

VmpcDirectToDiskRecorderScreen::VmpcDirectToDiskRecorderScreen(
    Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-direct-to-disk-recorder", layerIndex)
{
}

void VmpcDirectToDiskRecorderScreen::open()
{
    setSq(sequencer->getActiveSequenceIndex());
    displayRecord();
    displaySong();
    displayTime();
    displayOffline();
    displaySplitLR();
    displayRate();
}

void VmpcDirectToDiskRecorderScreen::turnWheel(const int i)
{

    const auto seq = sequencer->getSequence(sq).get();

    checkAllTimes(mpc, i, seq);

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "rate")
    {
        setSampleRate(sampleRate + i);
    }
    if (focusedFieldName == "record")
    {
        setRecord(record + i);
    }
    else if (focusedFieldName == "sq")
    {
        setSq(sq + i);
    }
    else if (focusedFieldName == "song")
    {
        setSong(song + i);
    }
    else if (focusedFieldName == "split-lr")
    {
        setSplitLR(i > 0);
    }
    else if (focusedFieldName == "offline")
    {
        setOffline(i > 0);
    }
}

void VmpcDirectToDiskRecorderScreen::function(const int i)
{

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SequencerScreen);
            break;
        case 4:
        {
            const auto seq = sq;
            const std::vector rates{44100, 48000, 88200};
            auto rate = rates[sampleRate];

            //		if (!offline)
            rate = static_cast<int>(
                mpc.getEngineHost()->getAudioServer()->getSampleRate());

            const auto sequence = sequencer->getSequence(seq);
            seqLoopWasEnabled = sequence->isLoopEnabled();

            switch (record)
            {
                case 0:
                {
                    openScreenById(ScreenId::SequencerScreen);

                    if (seqLoopWasEnabled)
                    {
                        sequence->setLoopEnabled(false);
                    }

                    const auto lengthInFrames = SeqUtil::sequenceFrameLength(
                        sequence.get(), 0, sequence->getLastTick(), rate);
                    const auto recordingName =
                        sequence->getName() + "-" +
                        DirectToDiskSettings::getTimeStamp();
                    const auto settings =
                        std::make_unique<DirectToDiskSettings>(
                            lengthInFrames, splitStereoIntoLeftAndRightChannel,
                            rate, recordingName);

                    if (!mpc.getEngineHost()->prepareBouncing(settings.get()))
                    {
                        openScreenById(ScreenId::VmpcFileInUseScreen);
                    }
                    else
                    {
                        sequencer->getTransport()->playFromStart();
                    }

                    break;
                }
                case 1:
                {
                    openScreenById(ScreenId::SequencerScreen);

                    auto lengthInFrames =
                        SeqUtil::loopFrameLength(sequence.get(), rate);
                    const auto recordingName =
                        sequence->getName() + "-" +
                        DirectToDiskSettings::getTimeStamp();
                    const auto settings =
                        std::make_unique<DirectToDiskSettings>(
                            lengthInFrames, splitStereoIntoLeftAndRightChannel,
                            rate, recordingName);

                    if (seqLoopWasEnabled)
                    {
                        sequence->setLoopEnabled(false);
                    }

                    sequencer->getTransport()->setPosition(
                        Sequencer::ticksToQuarterNotes(
                            sequence->getLoopStart()));

                    if (!mpc.getEngineHost()->prepareBouncing(settings.get()))
                    {
                        openScreenById(ScreenId::VmpcFileInUseScreen);
                    }
                    else
                    {
                        sequencer->getTransport()->play();
                    }

                    break;
                }
                case 2:
                {
                    openScreenById(ScreenId::SequencerScreen);

                    auto lengthInFrames = SeqUtil::sequenceFrameLength(
                        sequence.get(), time0, time1, rate);
                    const auto recordingName =
                        sequence->getName() + "-" +
                        DirectToDiskSettings::getTimeStamp();
                    const auto settings =
                        std::make_unique<DirectToDiskSettings>(
                            lengthInFrames, splitStereoIntoLeftAndRightChannel,
                            rate, recordingName);

                    if (seqLoopWasEnabled)
                    {
                        sequence->setLoopEnabled(false);
                    }

                    sequencer->getTransport()->setPosition(
                        Sequencer::ticksToQuarterNotes(time0));

                    if (!mpc.getEngineHost()->prepareBouncing(settings.get()))
                    {
                        openScreenById(ScreenId::VmpcFileInUseScreen);
                    }
                    else
                    {
                        sequencer->getTransport()->play();
                    }

                    break;
                }
                case 3:
                {
                    const auto mpcSong = sequencer->getSong(song);

                    if (!mpcSong->isUsed())
                    {
                        return;
                    }

                    const auto lengthInFrames = SeqUtil::songFrameLength(
                        mpcSong.get(), sequencer.get(), rate);
                    const auto recordingName =
                        mpcSong->getName() + "-" +
                        DirectToDiskSettings::getTimeStamp();
                    const auto settings =
                        std::make_unique<DirectToDiskSettings>(
                            lengthInFrames, splitStereoIntoLeftAndRightChannel,
                            rate, recordingName);

                    openScreenById(ScreenId::SongScreen);

                    songLoopWasEnabled = mpcSong->isLoopEnabled();

                    if (songLoopWasEnabled)
                    {
                        mpcSong->setLoopEnabled(false);
                    }

                    if (!mpc.getEngineHost()->prepareBouncing(settings.get()))
                    {
                        openScreenById(ScreenId::VmpcFileInUseScreen);
                    }
                    else
                    {
                        sequencer->setSongModeEnabled(true);
                        sequencer->getTransport()->playFromStart();
                    }

                    break;
                }
                case 4:
                    openScreenById(ScreenId::VmpcRecordJamScreen);
                    break;
            }
        }
    }
}

void VmpcDirectToDiskRecorderScreen::setSampleRate(const int rate)
{
    if (rate < 0 || rate > 2)
    {
        return;
    }

    sampleRate = rate;
    displayRate();
}

void VmpcDirectToDiskRecorderScreen::setRecord(const int i)
{
    if (i < 0 || i > 4)
    {
        return;
    }

    record = i;

    displayRecord();
    displaySq();
    displaySong();
    displayTime();
    displayOffline();
}

void VmpcDirectToDiskRecorderScreen::setSq(const int i)
{
    if (i < 0 || i > 98)
    {
        return;
    }

    sq = i;

    setTime0(0);

    const auto s = sequencer->getSequence(sq);

    if (s->isUsed())
    {
        setTime1(s->getLastTick());
    }
    else
    {
        setTime1(0);
    }

    displaySq();
}

void VmpcDirectToDiskRecorderScreen::setSong(const int i)
{
    if (i < 0 || i > 4)
    {
        return;
    }

    song = i;
    displaySong();
}

void VmpcDirectToDiskRecorderScreen::setOffline(const bool b)
{
    offline = b;
    displayOffline();
    displayRate();
}

void VmpcDirectToDiskRecorderScreen::setSplitLR(const bool b)
{
    splitStereoIntoLeftAndRightChannel = b;
    displaySplitLR();
}

void VmpcDirectToDiskRecorderScreen::displayRate() const
{
    findField("rate")->Hide(true);
    findLabel("rate")->Hide(true);
    //	findField("rate")->Hide(!offline);
    //	findLabel("rate")->Hide(!offline);

    //	if (!offline)
    //		return;
    //
    //    std::vector<std::string> rates{ "44.1", "48.0", "88.2" };
    //    std::string rate =
    //    Util::replaceDotWithSmallSpaceDot(rates[sampleRate]);
    //	findField("rate")->setText(rate);
}

void VmpcDirectToDiskRecorderScreen::displaySong() const
{
    findField("song")->Hide(record != 3);
    findLabel("song")->Hide(record != 3);

    if (record != 3)
    {
        return;
    }
    const auto songName = sequencer->getSong(song)->getName();
    findField("song")->setText(
        StrUtil::padLeft(std::to_string(song + 1), "0", 2) + "-" + songName);
}

void VmpcDirectToDiskRecorderScreen::displayOffline() const
{
    findField("offline")->Hide(record == 4);
    findLabel("offline")->Hide(record == 4);

    if (record != 4)
    {
        findField("offline")->setText(offline ? "YES" : "NO");
    }
}

void VmpcDirectToDiskRecorderScreen::displaySplitLR() const
{
    findField("split-lr")
        ->setText(splitStereoIntoLeftAndRightChannel ? "YES" : "NO");
}

void VmpcDirectToDiskRecorderScreen::displayRecord() const
{
    findField("record")->setText(recordNames[record]);
}

void VmpcDirectToDiskRecorderScreen::displaySq() const
{
    const auto visible = record >= 0 && record <= 2;

    findField("sq")->Hide(!visible);
    findLabel("sq")->Hide(!visible);

    if (!visible)
    {
        return;
    }

    const auto seqName = sequencer->getSequence(sq)->getName();
    findField("sq")->setText(StrUtil::padLeft(std::to_string(sq + 1), "0", 2) +
                             "-" + seqName);
}

void VmpcDirectToDiskRecorderScreen::displayTime()
{
    const auto invisible = record != 2;

    for (int i = 0; i < 6; i++)
    {
        findField("time" + std::to_string(i))->Hide(invisible);
        findLabel("time" + std::to_string(i))->Hide(invisible);
    }

    if (invisible)
    {
        return;
    }

    const auto sequence = sequencer->getSequence(sq);

    findField("time0")->setTextPadded(
        SeqUtil::getBar(sequence.get(), time0) + 1, "0");
    findField("time1")->setTextPadded(
        SeqUtil::getBeat(sequence.get(), time0) + 1, "0");
    findField("time2")->setTextPadded(SeqUtil::getClock(sequence.get(), time0),
                                      "0");
    findField("time3")->setTextPadded(
        SeqUtil::getBar(sequence.get(), time1) + 1, "0");
    findField("time4")->setTextPadded(
        SeqUtil::getBeat(sequence.get(), time1) + 1, "0");
    findField("time5")->setTextPadded(SeqUtil::getClock(sequence.get(), time1),
                                      "0");
}

bool VmpcDirectToDiskRecorderScreen::isOffline() const
{
    return offline;
}

int VmpcDirectToDiskRecorderScreen::getSampleRate() const
{
    return sampleRate;
}

int VmpcDirectToDiskRecorderScreen::getRecord() const
{
    return record;
}
