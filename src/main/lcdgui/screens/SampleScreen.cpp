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
    addReactiveBinding(
        {[&]() -> std::pair<float, float>
         {
             return {currentBufferPeakL.load(std::memory_order_relaxed),
                     currentBufferPeakR.load(std::memory_order_relaxed)};
         },
         [&](auto)
         {
             updateVU();
         }});
}

void SampleScreen::open()
{
    ls.lock()->getCurrentBackground()->setBackgroundName("sample");

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

            holdPeakL.store(0.f, std::memory_order_relaxed);
            holdPeakR.store(0.f, std::memory_order_relaxed);
            break;
        case 4:
            if (mpc.getEngineHost()->isRecordingSound())
            {
                mpc.getEngineHost()->stopSoundRecorder(true);
            }
            else if (mpc.getEngineHost()->getSoundRecorder()->isArmed())
            {
                mpc.getEngineHost()->getSoundRecorder()->setArmed(false);
                sampler.lock()->deleteSound(sampler.lock()->getSoundCount() -
                                            1);
            }
            break;
        case 5:
        {
            const auto engineHost = mpc.getEngineHost();

            if (engineHost->isRecordingSound())
            {
                engineHost->stopSoundRecorder();
                return;
            }

            if (engineHost->getSoundRecorder()->isArmed())
            {
                engineHost->startRecordingSound();
            }
            else
            {
                const auto sound = sampler.lock()->addSound();

                if (!sound)
                {
                    return;
                }

                sound->setName(sampler.lock()->addOrIncreaseNumber("sound1"));
                const auto lengthInFrames = time * (44100 * 0.1);
                engineHost->getSoundRecorder()->prepare(
                    sound, lengthInFrames,
                    engineHost->getAudioServer()->getSampleRate());
                engineHost->getSoundRecorder()->setArmed(true);
            }

            break;
        }
        default:
            break;
    }
}

void SampleScreen::turnWheel(const int i)
{
    if (const auto engineHost = mpc.getEngineHost();
        !engineHost->isRecordingSound())
    {
        if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
            focusedFieldName == "input")
        {
            setInput(input + i);
        }
        else if (focusedFieldName == "threshold")
        {
            setThreshold(threshold.load(std::memory_order_relaxed) + i);
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
    threshold.store(std::clamp(i, -64, 0), std::memory_order_relaxed);
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
    const auto currentThreshold = threshold.load(std::memory_order_relaxed);
    const auto thresholdText =
        currentThreshold == -64 ? u8"-\u00D9\u00DA" : std::to_string(currentThreshold);
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

    int peaklValue = static_cast<int>(
        floor(log10(holdPeakL.load(std::memory_order_relaxed)) * 20));
    int peakrValue = static_cast<int>(
        floor(log10(holdPeakR.load(std::memory_order_relaxed)) * 20));

    int levell = static_cast<int>(
        floor(log10(currentBufferPeakL.load(std::memory_order_relaxed)) * 20));
    int levelr = static_cast<int>(
        floor(log10(currentBufferPeakR.load(std::memory_order_relaxed)) * 20));

    const auto currentThreshold = threshold.load(std::memory_order_relaxed);

    for (int i = 0; i < 34; i++)
    {
        std::string l = " ";
        std::string r = " ";
        bool normall = vuPosToDb[i] <= levell;
        bool normalr = vuPosToDb[i] <= levelr;

        bool thresholdHit = currentThreshold >= vuPosToDb[i] &&
                            (i == 33 || currentThreshold < vuPosToDb[i + 1]);

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

    findLabel("vuleft")->setText(mode == 0 || mode == 2
                                     ? lString
                                     : "                                  ");
    findLabel("vuright")->setText(mode == 1 || mode == 2
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
    currentBufferPeakL.store(peakToUse.first, std::memory_order_relaxed);
    currentBufferPeakR.store(peakToUse.second, std::memory_order_relaxed);

    float oldL = holdPeakL.load(std::memory_order_relaxed);
    while (peakToUse.first > oldL &&
           !holdPeakL.compare_exchange_weak(oldL, peakToUse.first,
                                            std::memory_order_relaxed))
        ;

    float oldR = holdPeakR.load(std::memory_order_relaxed);
    while (peakToUse.second > oldR &&
           !holdPeakR.compare_exchange_weak(oldR, peakToUse.second,
                                            std::memory_order_relaxed))
        ;
}
