#include "VmpcDirectToDiskRecorderScreen.hpp"

#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/DirectToDiskSettings.hpp"

#include "sequencer/Song.hpp"
#include "sequencer/SeqUtil.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"

#include "lcdgui/screens/SongScreen.hpp"

using namespace mpc;
using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace mpc::sequencer;

VmpcDirectToDiskRecorderScreen::VmpcDirectToDiskRecorderScreen(mpc::Mpc &mpc, const int layerIndex)
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

void VmpcDirectToDiskRecorderScreen::turnWheel(int i)
{

    auto seq = sequencer->getSequence(sq).get();

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

void VmpcDirectToDiskRecorderScreen::function(int i)
{

    switch (i)
    {
        case 3:
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            break;
        case 4:
        {
            auto seq = sq;
            std::vector<int> rates{44100, 48000, 88200};
            auto rate = rates[sampleRate];

            //		if (!offline)
            rate = static_cast<int>(mpc.getAudioMidiServices()->getAudioServer()->getSampleRate());

            auto sequence = sequencer->getSequence(seq);
            seqLoopWasEnabled = sequence->isLoopEnabled();

            switch (record)
            {
                case 0:
                {
                    mpc.getLayeredScreen()->openScreen<SequencerScreen>();

                    if (seqLoopWasEnabled)
                    {
                        sequence->setLoopEnabled(false);
                    }

                    const auto lengthInFrames = SeqUtil::sequenceFrameLength(sequence.get(), 0, sequence->getLastTick(), rate);
                    const auto recordingName = sequence->getName() + "-" + DirectToDiskSettings::getTimeStamp();
                    const auto settings = std::make_unique<DirectToDiskSettings>(
                        lengthInFrames,
                        splitStereoIntoLeftAndRightChannel,
                        rate,
                        recordingName);

                    if (!mpc.getAudioMidiServices()->prepareBouncing(settings.get()))
                    {
                        mpc.getLayeredScreen()->openScreen<VmpcFileInUseScreen>();
                    }
                    else
                    {
                        sequencer->playFromStart();
                    }

                    break;
                }
                case 1:
                {
                    mpc.getLayeredScreen()->openScreen<SequencerScreen>();

                    auto lengthInFrames = SeqUtil::loopFrameLength(sequence.get(), rate);
                    const auto recordingName = sequence->getName() + "-" + DirectToDiskSettings::getTimeStamp();
                    auto settings = std::make_unique<DirectToDiskSettings>(
                        lengthInFrames,
                        splitStereoIntoLeftAndRightChannel,
                        rate,
                        recordingName);

                    if (seqLoopWasEnabled)
                    {
                        sequence->setLoopEnabled(false);
                    }

                    sequencer->move(Sequencer::ticksToQuarterNotes(sequence->getLoopStart()));

                    if (!mpc.getAudioMidiServices()->prepareBouncing(settings.get()))
                    {
                        mpc.getLayeredScreen()->openScreen<VmpcFileInUseScreen>();
                    }
                    else
                    {
                        sequencer->play();
                    }

                    break;
                }
                case 2:
                {
                    mpc.getLayeredScreen()->openScreen<SequencerScreen>();

                    auto lengthInFrames = SeqUtil::sequenceFrameLength(sequence.get(), time0, time1, rate);
                    const auto recordingName = sequence->getName() + "-" + DirectToDiskSettings::getTimeStamp();
                    auto settings = std::make_unique<DirectToDiskSettings>(
                        lengthInFrames,
                        splitStereoIntoLeftAndRightChannel,
                        rate,
                        recordingName);

                    if (seqLoopWasEnabled)
                    {
                        sequence->setLoopEnabled(false);
                    }

                    sequencer->move(Sequencer::ticksToQuarterNotes(time0));

                    if (!mpc.getAudioMidiServices()->prepareBouncing(settings.get()))
                    {
                        mpc.getLayeredScreen()->openScreen<VmpcFileInUseScreen>();
                    }
                    else
                    {
                        sequencer->play();
                    }

                    break;
                }
                case 3:
                {
                    auto mpcSong = sequencer->getSong(song);

                    if (!mpcSong->isUsed())
                    {
                        return;
                    }

                    const auto lengthInFrames = SeqUtil::songFrameLength(mpcSong.get(), sequencer.get(), rate);
                    const auto recordingName = mpcSong->getName() + "-" + DirectToDiskSettings::getTimeStamp();
                    auto settings = std::make_unique<DirectToDiskSettings>(
                        lengthInFrames,
                        splitStereoIntoLeftAndRightChannel,
                        rate,
                        recordingName);

                    mpc.getLayeredScreen()->openScreen<SongScreen>();

                    songLoopWasEnabled = mpcSong->isLoopEnabled();

                    if (songLoopWasEnabled)
                    {
                        mpcSong->setLoopEnabled(false);
                    }

                    if (!mpc.getAudioMidiServices()->prepareBouncing(settings.get()))
                    {
                        mpc.getLayeredScreen()->openScreen<VmpcFileInUseScreen>();
                    }
                    else
                    {
                        sequencer->setSongModeEnabled(true);
                        sequencer->playFromStart();
                    }

                    break;
                }
                case 4:
                    mpc.getLayeredScreen()->openScreen<VmpcRecordJamScreen>();
                    break;
            }
        }
    }
}

void VmpcDirectToDiskRecorderScreen::setSampleRate(int rate)
{
    if (rate < 0 || rate > 2)
    {
        return;
    }

    sampleRate = rate;
    displayRate();
}

void VmpcDirectToDiskRecorderScreen::setRecord(int i)
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

void VmpcDirectToDiskRecorderScreen::setSq(int i)
{
    if (i < 0 || i > 98)
    {
        return;
    }

    sq = i;

    setTime0(0);

    auto s = sequencer->getSequence(sq);

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

void VmpcDirectToDiskRecorderScreen::setSong(int i)
{
    if (i < 0 || i > 4)
    {
        return;
    }

    song = i;
    displaySong();
}

void VmpcDirectToDiskRecorderScreen::setOffline(bool b)
{
    offline = b;
    displayOffline();
    displayRate();
}

void VmpcDirectToDiskRecorderScreen::setSplitLR(bool b)
{
    splitStereoIntoLeftAndRightChannel = b;
    displaySplitLR();
}

void VmpcDirectToDiskRecorderScreen::displayRate()
{
    findField("rate")->Hide(true);
    findLabel("rate")->Hide(true);
    //	findField("rate")->Hide(!offline);
    //	findLabel("rate")->Hide(!offline);

    //	if (!offline)
    //		return;
    //
    //    std::vector<std::string> rates{ "44.1", "48.0", "88.2" };
    //    std::string rate = Util::replaceDotWithSmallSpaceDot(rates[sampleRate]);
    //	findField("rate")->setText(rate);
}

void VmpcDirectToDiskRecorderScreen::displaySong()
{
    findField("song")->Hide(record != 3);
    findLabel("song")->Hide(record != 3);

    if (record != 3)
    {
        return;
    }
    auto songName = sequencer->getSong(song)->getName();
    findField("song")->setText(StrUtil::padLeft(std::to_string(song + 1), "0", 2) + "-" + songName);
}

void VmpcDirectToDiskRecorderScreen::displayOffline()
{
    findField("offline")->Hide(record == 4);
    findLabel("offline")->Hide(record == 4);

    if (record != 4)
    {
        findField("offline")->setText(offline ? "YES" : "NO");
    }
}

void VmpcDirectToDiskRecorderScreen::displaySplitLR()
{
    findField("split-lr")->setText(splitStereoIntoLeftAndRightChannel ? "YES" : "NO");
}

void VmpcDirectToDiskRecorderScreen::displayRecord()
{
    findField("record")->setText(recordNames[record]);
}

void VmpcDirectToDiskRecorderScreen::displaySq()
{
    auto visible = (record >= 0 && record <= 2);

    findField("sq")->Hide(!visible);
    findLabel("sq")->Hide(!visible);

    if (!visible)
    {
        return;
    }

    auto seqName = sequencer->getSequence(sq)->getName();
    findField("sq")->setText(StrUtil::padLeft(std::to_string(sq + 1), "0", 2) + "-" + seqName);
}

void VmpcDirectToDiskRecorderScreen::displayTime()
{
    auto invisible = record != 2;

    for (int i = 0; i < 6; i++)
    {
        findField("time" + std::to_string(i))->Hide(invisible);
        findLabel("time" + std::to_string(i))->Hide(invisible);
    }

    if (invisible)
    {
        return;
    }

    auto sequence = sequencer->getSequence(sq);

    findField("time0")->setTextPadded(SeqUtil::getBar(sequence.get(), time0) + 1, "0");
    findField("time1")->setTextPadded(SeqUtil::getBeat(sequence.get(), time0) + 1, "0");
    findField("time2")->setTextPadded(SeqUtil::getClock(sequence.get(), time0), "0");
    findField("time3")->setTextPadded(SeqUtil::getBar(sequence.get(), time1) + 1, "0");
    findField("time4")->setTextPadded(SeqUtil::getBeat(sequence.get(), time1) + 1, "0");
    findField("time5")->setTextPadded(SeqUtil::getClock(sequence.get(), time1), "0");
}

bool VmpcDirectToDiskRecorderScreen::isOffline()
{
    return offline;
}

int VmpcDirectToDiskRecorderScreen::getSampleRate()
{
    return sampleRate;
}

int VmpcDirectToDiskRecorderScreen::getRecord()
{
    return record;
}
