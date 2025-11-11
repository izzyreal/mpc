#include "SampleScreen.hpp"

#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
#include "audiomidi/SoundRecorder.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens;

SampleScreen::SampleScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "sample", layerIndex)
{
    addReactiveBinding(
        {[&]() -> std::pair<bool, bool>
         {
             return {mpc.getEngineHost()->isRecordingSound(),
                     mpc.getEngineHost()->getSoundRecorder()->isArmed()};
         },
         [&](auto isRecordingAndisArmed)
         {
             const auto b = findBackground();
             if (isRecordingAndisArmed.first)
             {
                 b->setBackgroundName("recording");
             }
             else if (isRecordingAndisArmed.second)
             {
                 b->setBackgroundName("waiting-for-input-signal");
             }
             else
             {
                 b->setBackgroundName("sample");
             }
         }});
    addReactiveBinding({[&]() -> std::pair<float, float>
                        {
                            return {currentBufferPeakL, currentBufferPeakR};
                        },
                        [&](auto)
                        {
                            updateVU();
                        }});
}

void SampleScreen::open()
{
    ls->getCurrentBackground()->setBackgroundName("sample");

    displayInput();
    displayThreshold();
    displayMode();
    displayTime();
    displayMonitor();
    displayPreRec();
}

void SampleScreen::left()
{
    if (mpc.getEngineHost()->isRecordingSound())
    {
        return;
    }

    ScreenComponent::left();
}

void SampleScreen::right()
{
    if (mpc.getEngineHost()->isRecordingSound())
    {
        return;
    }

    ScreenComponent::right();
}

void SampleScreen::up()
{
    if (mpc.getEngineHost()->isRecordingSound())
    {
        return;
    }

    ScreenComponent::up();
}

void SampleScreen::down()
{
    if (mpc.getEngineHost()->isRecordingSound())
    {
        return;
    }

    ScreenComponent::down();
}

void SampleScreen::openWindow()
{
    openScreenById(ScreenId::SoundMemoryScreen);
}

void SampleScreen::function(const int i)
{
    switch (i)
    {
        case 0:
            if (mpc.getEngineHost()->isRecordingSound())
            {
                return;
            }

            peak.first = 0.f;
            peak.second = 0.f;
            break;
        case 4:
            if (mpc.getEngineHost()->isRecordingSound())
            {
                mpc.getEngineHost()->stopSoundRecorder(true);
            }
            else if (mpc.getEngineHost()->getSoundRecorder()->isArmed())
            {
                mpc.getEngineHost()->getSoundRecorder()->setArmed(false);
                sampler->deleteSound(sampler->getSoundCount() - 1);
            }
            break;
        case 5:
        {
            const auto ams = mpc.getEngineHost();

            if (ams->isRecordingSound())
            {
                ams->stopSoundRecorder();
                return;
            }

            if (ams->getSoundRecorder()->isArmed())
            {
                ams->startRecordingSound();
            }
            else
            {
                const auto sound = sampler->addSound();

                if (!sound)
                {
                    return;
                }

                sound->setName(sampler->addOrIncreaseNumber("sound1"));
                const auto lengthInFrames = time * (44100 * 0.1);
                ams->getSoundRecorder()->prepare(
                    sound, lengthInFrames,
                    ams->getAudioServer()->getSampleRate());
                ams->getSoundRecorder()->setArmed(true);
            }

            break;
        }
        default:
            break;
    }
}

void SampleScreen::turnWheel(const int i)
{
    const auto ams = mpc.getEngineHost();

    if (!ams->isRecordingSound())
    {
        const auto focusedFieldName = getFocusedFieldNameOrThrow();

        if (focusedFieldName == "input")
        {
            setInput(input + i);
        }
        else if (focusedFieldName == "threshold")
        {
            setThreshold(threshold + i);
        }
        else if (focusedFieldName == "mode")
        {
            setMode(mode + i);
        }
        else if (focusedFieldName == "time")
        {
            setTime(time + i);
        }
        else if (focusedFieldName == "monitor")
        {
            setMonitor(monitor + i);
            const bool muteMonitor = monitor == 0;
            mpc.getEngineHost()->muteMonitor(muteMonitor);
        }
        else if (focusedFieldName == "prerec")
        {
            setPreRec(preRec + i);
        }
    }
}

void SampleScreen::setInput(const int i)
{
    input = std::clamp(i, 0, 1);
    displayInput();
}

void SampleScreen::setThreshold(const int i)
{
    threshold = std::clamp(i, -64, 0);
    displayThreshold();
}

void SampleScreen::setMode(const int i)
{
    mode = std::clamp(i, 0, 2);
    displayMode();
}

void SampleScreen::setTime(const int i)
{
    time = std::clamp(i, 0, 3786);
    displayTime();
}

void SampleScreen::setMonitor(const int i)
{
    monitor = std::clamp(i, 0, 5);
    displayMonitor();
}

void SampleScreen::setPreRec(const int i)
{
    preRec = std::clamp(i, 0, 100);
    displayPreRec();
}

void SampleScreen::displayInput() const
{
    findField("input")->setText(inputNames[input]);
}

void SampleScreen::displayThreshold() const
{
    const auto thresholdText =
        threshold == -64 ? u8"-\u00D9\u00DA" : std::to_string(threshold);
    findField("threshold")->setTextPadded(thresholdText);
}

void SampleScreen::displayMode() const
{
    findField("mode")->setText(modeNames[mode]);
}

void SampleScreen::displayTime() const
{
    auto timeText = std::to_string(time);
    timeText = timeText.substr(0, timeText.length() - 1) + "." +
               timeText.substr(timeText.length() - 1);
    findField("time")->setTextPadded(timeText);
}

void SampleScreen::displayMonitor() const
{
    findField("monitor")->setText(monitorNames[monitor]);
}

void SampleScreen::displayPreRec() const
{
    findField("prerec")->setTextPadded(preRec);
}

void SampleScreen::updateVU()
{
    std::string lString;
    std::string rString;

    int peaklValue = static_cast<int>(floor(log10(peak.first) * 20));
    int peakrValue = static_cast<int>(floor(log10(peak.second) * 20));

    int levell = static_cast<int>(floor(log10(currentBufferPeakL) * 20));
    int levelr = static_cast<int>(floor(log10(currentBufferPeakR) * 20));

    for (int i = 0; i < 34; i++)
    {
        std::string l = " ";
        std::string r = " ";
        bool normall = vuPosToDb[i] <= levell;
        bool normalr = vuPosToDb[i] <= levelr;

        bool thresholdHit = threshold >= vuPosToDb[i] &&
                            (i == 33 || threshold < vuPosToDb[i + 1]);

        bool peakl = peaklValue >= vuPosToDb[i] &&
                     (i == 33 || peaklValue < vuPosToDb[i + 1]);
        bool peakr = peakrValue >= vuPosToDb[i] &&
                     (i == 33 || peakrValue < vuPosToDb[i + 1]);

        if (thresholdHit && peakl)
        {
            l = vu_peak_threshold;
        }
        if (thresholdHit && peakr)
        {
            r = vu_peak_threshold;
        }

        if (thresholdHit && normall && !peakl)
        {
            l = vu_normal_threshold;
        }
        if (thresholdHit && normalr && !peakr)
        {
            r = vu_normal_threshold;
        }

        if (thresholdHit && !peakl && !normall)
        {
            l = vu_threshold;
        }
        if (thresholdHit && !peakr && !normalr)
        {
            r = vu_threshold;
        }

        if (normall && !peakl && !thresholdHit)
        {
            l = vu_normal;
        }
        if (normalr && !peakr && !thresholdHit)
        {
            r = vu_normal;
        }

        if (peakl && !thresholdHit)
        {
            l = vu_peak;
        }
        if (peakr && !thresholdHit)
        {
            r = vu_peak;
        }

        if (peakl && thresholdHit && levell == 33)
        {
            l = vu_peak_threshold_normal;
        }
        if (peakr && thresholdHit && levelr == 33)
        {
            r = vu_peak_threshold_normal;
        }

        lString += l;
        rString += r;
    }

    findLabel("vuleft")->setText((mode == 0 || mode == 2)
                                     ? lString
                                     : "                                  ");
    findLabel("vuright")->setText((mode == 1 || mode == 2)
                                      ? rString
                                      : "                                  ");
}

int SampleScreen::getMode() const
{
    return mode;
}

int SampleScreen::getMonitor() const
{
    return monitor;
}

void SampleScreen::setCurrentBufferPeak(
    const std::pair<float, float> &peakToUse)
{
    currentBufferPeakL.store(peakToUse.first);
    currentBufferPeakR.store(peakToUse.second);
    peak.first = std::max(currentBufferPeakL.load(), peak.first);
    peak.second = std::max(currentBufferPeakR.load(), peak.second);
}
