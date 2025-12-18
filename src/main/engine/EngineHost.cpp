#include "EngineHost.hpp"

#include "Mpc.hpp"

#include "lcdgui/screens/VmpcMidiScreen.hpp"
#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"

#include "audiomidi/DirectToDiskSettings.hpp"
#include "audiomidi/DiskRecorder.hpp"
#include "audiomidi/MonitorInputAdapter.hpp"
#include "audiomidi/SoundPlayer.hpp"
#include "audiomidi/SoundRecorder.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"

#include "engine/SequencerPlaybackEngine.hpp"
#include "engine/Voice.hpp"
#include "engine/VoiceUtil.hpp"
#include "engine/MixerInterconnection.hpp"
#include "engine/FaderControl.hpp"
#include "engine/PreviewSoundPlayer.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "engine/audio/server/RealTimeAudioServer.hpp"
#include "engine/audio/server/CompoundAudioClient.hpp"
#include "engine/control/CompoundControl.hpp"
#include "engine/control/BooleanControl.hpp"
#include "engine/audio/mixer/MixerControls.hpp"
#include "engine/audio/mixer/AudioMixer.hpp"
#include "engine/audio/mixer/MixerControlsFactory.hpp"
#include "engine/audio/mixer/MainMixControls.hpp"

#include "performance/PerformanceManager.hpp"

#include "hardware/Hardware.hpp"

#include "sampler/Sampler.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Song.hpp"

#include <string>

using namespace mpc;
using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;

using namespace mpc::engine::audio::server;
using namespace mpc::engine::audio::core;
using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::control;
using namespace mpc::engine;

using namespace mpc::sequencer;

EngineHost::EngineHost(Mpc &mpcToUse)
    : postToAudioThread(
          [&](utils::Task &&task)
          {
              audioTasks.post(task);
          }),
      mpc(mpcToUse)
{
    VoiceUtil::freqTable();
}

void EngineHost::start()
{
    realTimeAudioServer = std::make_shared<RealTimeAudioServer>();
    nonRealTimeAudioServer =
        std::make_shared<NonRealTimeAudioServer>(this, realTimeAudioServer);
    soundRecorder = std::make_shared<SoundRecorder>(mpc);
    soundPlayer = std::make_shared<SoundPlayer>();

    setupMixer();

    createSynth();

    inputProcess = realTimeAudioServer->openAudioInput("RECORD IN");
    monitorInputAdapter =
        std::make_shared<MonitorInputAdapter>(mpc, inputProcess.get());

    const std::vector<std::string> outputNames{
        "STEREO OUT", "ASSIGNABLE MIX OUT 1/2", "ASSIGNABLE MIX OUT 3/4",
        "ASSIGNABLE MIX OUT 5/6", "ASSIGNABLE MIX OUT 7/8"};

    for (int i = 0; i < 5; i++)
    {
        outputProcesses.push_back(
            realTimeAudioServer->openAudioOutput(outputNames[i]));
    }

    connectVoices();

    initializeDiskRecorders();

    mixer->getStrip(std::string("66"))->setDirectOutputProcess(soundRecorder);
    mixer->getStrip(std::string("67"))->setInputProcess(soundPlayer);
    const auto sc = mixer->getMixerControls()->getStripControls("67");
    const auto mmc =
        std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
    std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))
        ->setValue(static_cast<float>(100));

    mixer->getStrip("66")->setInputProcess(monitorInputAdapter);

    noteRepeatProcessor = std::make_shared<NoteRepeatProcessor>(
        mpc.getEventHandler().get(), mpc.getSequencer(), mpc.getSampler(),
        mixer, mpc.screens->get<ScreenId::Assign16LevelsScreen>(),
        mpc.screens->get<ScreenId::MixerSetupScreen>(),
        mpc.getPerformanceManager(), mpc.getHardware()->getSlider(), &voices,
        mixerConnections,
        [controller = mpc.clientEventController]
        {
            return controller->isFullLevelEnabled();
        },
        [controller = mpc.clientEventController]
        {
            return controller->isSixteenLevelsEnabled();
        });

    sequencerPlaybackEngine = std::make_shared<SequencerPlaybackEngine>(
        mpc.getPerformanceManager().lock().get(), this,
        [&]
        {
            return mpc.getMidiOutput();
        },
        mpc.getSequencer().get(), mpc.getClock(), mpc.getLayeredScreen(),
        [&]
        {
            return isBouncing();
        },
        [&]
        {
            return nonRealTimeAudioServer->getSampleRate();
        },
        [&]
        {
            return mpc.clientEventController->isRecMainWithoutPlaying();
        },
        [sampler = mpc.getSampler()](const int velo, const int frameOffset)
        {
            sampler->playMetronome(velo, frameOffset);
        },
        [&]
        {
            return mpc.screens;
        },
        [&]
        {
            return mpc.clientEventController->clientHardwareEventController
                ->isNoteRepeatLockedOrPressed();
        },
        noteRepeatProcessor,
        [&]
        {
            return !nonRealTimeAudioServer->isRealTime();
        });

    compoundAudioClient = std::make_shared<CompoundAudioClient>();
    compoundAudioClient->add(sequencerPlaybackEngine.get());
    compoundAudioClient->add(mixer.get());
    nonRealTimeAudioServer->setClient(compoundAudioClient);
}

void EngineHost::postSamplePreciseTaskToAudioThread(
    concurrency::SamplePreciseTask &task)
{
    preciseTasks.post(std::move(task));
}

void EngineHost::flushNoteOffs()
{
    postToAudioThread(utils::Task(
        [this]
        {
            preciseTasks.flushNoteOffs();
        }));
}

void EngineHost::prepareProcessBlock(const int nFrames)
{
    audioTasks.drain();
    preciseTasks.processTasks(nFrames);
    mpc.getSequencer()->getStateManager()->drainQueue();
    mpc.getPerformanceManager().lock()->drainQueue();
}

void EngineHost::setMonitorLevel(const int8_t level) const
{
    const auto sc = mixer->getMixerControls()->getStripControls("66");
    const auto mmc =
        std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
    std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))
        ->setValue(level);
}

void EngineHost::muteMonitor(const bool mute) const
{
    const auto sc = mixer->getMixerControls()->getStripControls("66");
    const auto mmc =
        std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
    const auto mc =
        std::dynamic_pointer_cast<BooleanControl>(mmc->find("Mute"));
    mc->setValue(mute);
}

std::shared_ptr<SoundRecorder> EngineHost::getSoundRecorder()
{
    return soundRecorder;
}

std::shared_ptr<SoundPlayer> EngineHost::getSoundPlayer()
{
    return soundPlayer;
}

std::shared_ptr<NonRealTimeAudioServer> EngineHost::getAudioServer() const
{
    return nonRealTimeAudioServer;
}

void EngineHost::setupMixer()
{
    mixerControls = std::make_shared<MixerControls>("", 1.f);

    // AUX#1 - #4 represent ASSIGNABLE MIX OUT 1/2, 3/4, 5/6 and 7/8
    mixerControls->createAuxBusControls("AUX#1");
    mixerControls->createAuxBusControls("AUX#2");
    mixerControls->createAuxBusControls("AUX#3");
    mixerControls->createAuxBusControls("AUX#4");

    // L/R represents STEREO OUT L/R
    MixerControlsFactory::createBusStrips(
        std::dynamic_pointer_cast<MixerControls>(mixerControls), "L-R");

    /*
     * There are 32 voices. Each voice has one channel for mixing to STEREO OUT
     * L/R, and one channel for mixing to an ASSIGNABLE MIX OUT. These are
     * strips 1-64. There's one channel for the PreviewSoundPlayer, which plays
     * the metronome, preview and playX sounds. This is strip 65. Finally,
     * there's one channel to receive and monitor sampler input, this is strip
     * 66, and one for playing quick previews, on strip 67. Hence, nMixerChans =
     * 67
     */
    constexpr int nMixerChans = 67;
    MixerControlsFactory::createChannelStrips(mixerControls, nMixerChans);
    mixer = std::make_shared<AudioMixer>(mixerControls, nonRealTimeAudioServer);
    muteMonitor(true);
    setAssignableMixOutLevels();
}

void EngineHost::setMainLevel(const int i) const
{
    const auto sc = mixer->getMixerControls()->getStripControls("L-R");
    const auto cc =
        std::dynamic_pointer_cast<CompoundControl>(sc->find("Main"));
    std::dynamic_pointer_cast<FaderControl>(cc->find("Level"))->setValue(i);
}

int EngineHost::getMainLevel() const
{
    const auto sc = mixer->getMixerControls()->getStripControls("L-R");
    const auto cc =
        std::dynamic_pointer_cast<CompoundControl>(sc->find("Main"));
    const auto val =
        std::dynamic_pointer_cast<FaderControl>(cc->find("Level"))->getValue();
    return static_cast<int>(val);
}

void EngineHost::setRecordLevel(const int8_t i) const
{
    soundRecorder->setInputGain(i);
    setMonitorLevel(i);
}

int8_t EngineHost::getRecordLevel() const
{
    return soundRecorder->getInputGain();
}

void EngineHost::setAssignableMixOutLevels() const
{
    /*
     * We have to make sure the ASSIGNABLE MIX OUTs are audible. They're fixed
     * at value 100.
     */
    for (auto j = 1; j <= 4; j++)
    {
        const std::string name = "AUX#" + std::to_string(j);
        const auto sc = mixer->getMixerControls()->getStripControls(name);
        const auto cc =
            std::dynamic_pointer_cast<CompoundControl>(sc->find(name));
        std::dynamic_pointer_cast<FaderControl>(cc->find("Level"))
            ->setValue(100);
    }
}

std::shared_ptr<PreviewSoundPlayer> EngineHost::getPreviewSoundPlayer() const
{
    return previewSoundPlayer;
}

void EngineHost::createSynth()
{
    basicVoice = std::make_shared<Voice>(65, true);

    for (int i = 0; i < 32; i++)
    {
        voices.emplace_back(std::make_shared<Voice>(i + 1, false));
    }

    previewSoundPlayer = std::make_shared<PreviewSoundPlayer>(
        mpc.getSampler(), mixer, basicVoice);
}

void EngineHost::connectVoices()
{
    for (auto j = 0; j < 32; j++)
    {
        const auto ams1 = mixer->getStrip(std::to_string(j + 1));
        auto voice = voices[j];
        ams1->setInputProcess(voice);
        mixerConnections.emplace_back(new MixerInterconnection(
            "con" + std::to_string(j), realTimeAudioServer.get()));
        const auto &mi = mixerConnections.back();
        ams1->setDirectOutputProcess(mi->getInputProcess());
        const auto ams2 = mixer->getStrip(std::to_string(j + 1 + 32));
        ams2->setInputProcess(mi->getOutputProcess());
    }

    previewSoundPlayer->connectVoice();
}

void EngineHost::initializeDiskRecorders()
{
    for (int i = 0; i < outputProcesses.size(); i++)
    {
        auto diskRecorder =
            std::make_shared<DiskRecorder>(mpc, outputProcesses[i].get(), i);

        diskRecorders.push_back(diskRecorder);

        if (i == 0)
        {
            mixer->getMainStrip()->setDirectOutputProcess(diskRecorders.back());
        }
        else
        {
            mixer->getStrip(std::string("AUX#" + std::to_string(i)))
                ->setDirectOutputProcess(diskRecorders.back());
        }
    }
}

void EngineHost::destroyServices() const
{
    nonRealTimeAudioServer->stop();
    closeIO();
    mixer->close();
    nonRealTimeAudioServer->close();
}

void EngineHost::closeIO() const
{
    realTimeAudioServer->closeAudioInput(inputProcess);

    for (auto &outputProcess : outputProcesses)
    {
        realTimeAudioServer->closeAudioOutput(outputProcess);
    }
}

bool EngineHost::startBouncing(const DirectToDiskSettings *settings)
{
    const auto destinationDirectory =
        mpc.paths->recordingsPath() / settings->recordingName;

    fs::create_directory(destinationDirectory);

    for (int i = 0; i < diskRecorders.size(); i++)
    {
        if (const auto diskRecorder = diskRecorders[i]; !diskRecorder->prepare(
                settings->lengthInFrames, settings->sampleRate,
                !settings->splitStereoIntoLeftAndRightChannel,
                destinationDirectory))
        {
            for (int j = 0; j < i; ++j)
            {
                diskRecorders[j]->unprepare();
            }

            onBounceStart = {};
            return false;
        }
    }

    bouncing.store(true);

    return true;
}

void EngineHost::stopBouncing()
{
    if (!bouncing.load())
    {
        return;
    }

    bouncing.store(false);

    const auto directToDiskRecorderScreen =
        mpc.screens->get<ScreenId::VmpcDirectToDiskRecorderScreen>();

    if (directToDiskRecorderScreen->seqLoopWasEnabled)
    {
        const auto seq =
            mpc.getSequencer()->getSequence(directToDiskRecorderScreen->sq);
        seq->setLoopEnabled(true);
        directToDiskRecorderScreen->seqLoopWasEnabled = false;
    }
    else if (directToDiskRecorderScreen->songLoopWasEnabled)
    {
        const auto song =
            mpc.getSequencer()->getSong(directToDiskRecorderScreen->song);
        song->setLoopEnabled(true);
        directToDiskRecorderScreen->songLoopWasEnabled = false;
    }

    postToAudioThread(utils::Task(
        [&]
        {
            mpc.getLayeredScreen()->postToUiThread(utils::Task(
                [ls = mpc.getLayeredScreen()]
                {
                    ls->openScreenById(ScreenId::VmpcRecordingFinishedScreen);
                }));
        }));
}

void EngineHost::stopBouncingEarly()
{
    if (!bouncing.load())
    {
        return;
    }

    for (const auto &recorder : diskRecorders)
    {
        recorder->stopEarly();
    }

    stopBouncing();
}

void EngineHost::startRecordingSound()
{
    recordingSound.store(true, std::memory_order_relaxed);
}

void EngineHost::stopSoundRecorder(const bool cancel)
{
    if (cancel)
    {
        soundRecorder->cancel();
    }

    recordingSound.store(false);
}

void EngineHost::finishPreviewSoundPlayerVoice() const
{
    postToAudioThread(utils::Task(
        [&]
        {
            getPreviewSoundPlayer()->finishVoice();
        }));
}

bool EngineHost::isBouncing() const
{
    return bouncing.load();
}

bool EngineHost::isRecordingSound() const
{
    return recordingSound.load(std::memory_order_relaxed);
}

// Should be called from the audio thread only!
void EngineHost::changeSoundRecorderStateIfRequired()
{
    if (wasRecordingSound && !soundRecorder->isRecording())
    {
        soundRecorder->stop();
        stopSoundRecorder();
        wasRecordingSound = false;
        return;
    }

    if (!wasRecordingSound && isRecordingSound())
    {
        wasRecordingSound = true;
        soundRecorder->start();
    }
    else if (wasRecordingSound && !isRecordingSound())
    {
        wasRecordingSound = false;
        soundRecorder->stop();
    }
}

// Should be called from the audio thread only!
void EngineHost::changeBounceStateIfRequired()
{
    const auto directToDiskRecorderScreen =
        mpc.screens->get<ScreenId::VmpcDirectToDiskRecorderScreen>();

    if (isBouncing() && !wasBouncing)
    {
        wasBouncing = true;

        if (directToDiskRecorderScreen->isOffline())
        {
            if (getAudioServer()->isRealTime())
            {
                mpc.getLayeredScreen()->postToUiThread(utils::Task(
                    [server = getAudioServer()]
                    {
                        server->setRealTime(false);
                    }));
            }
        }

        for (const auto &diskRecorder : diskRecorders)
        {
            diskRecorder->start();
        }

        onBounceStart();
    }
    else if (!isBouncing() && wasBouncing)
    {
        wasBouncing = false;

        if (directToDiskRecorderScreen->isOffline())
        {
            if (!getAudioServer()->isRealTime())
            {
                mpc.getLayeredScreen()->postToUiThread(utils::Task(
                    [server = getAudioServer()]
                    {
                        server->setRealTime(true);
                    }));
            }
        }

        onBounceStart = {};
    }
}

// Should be called from the audio thread only!
void EngineHost::switchMidiControlMappingIfRequired() const
{
    if (const auto vmpcMidiScreen =
            mpc.screens->get<ScreenId::VmpcMidiScreen>();
        vmpcMidiScreen->shouldSwitch.load())
    {
        vmpcMidiScreen->activePreset = vmpcMidiScreen->switchToPreset;

        vmpcMidiScreen->shouldSwitch.store(false);

        if (mpc.getLayeredScreen()->getCurrentScreenName() == "vmpc-midi")
        {
            mpc.getScreen()->open();
        }
    }
}

void EngineHost::setMixerMasterLevel(const int8_t dbValue) const
{
    for (auto &v : voices)
    {
        v->setMasterLevel(dbValue);
    }
}

std::shared_ptr<AudioMixer> EngineHost::getMixer()
{
    return mixer;
}

std::vector<std::shared_ptr<Voice>> &EngineHost::getVoices()
{
    return voices;
}

std::vector<MixerInterconnection *> &EngineHost::getMixerConnections()
{
    return mixerConnections;
}

std::shared_ptr<SequencerPlaybackEngine>
EngineHost::getSequencerPlaybackEngine()
{
    return sequencerPlaybackEngine;
}
