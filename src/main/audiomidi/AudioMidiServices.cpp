#include "AudioMidiServices.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

#include "Mpc.hpp"
#include "audiomidi/DirectToDiskSettings.hpp"
#include "audiomidi/DiskRecorder.hpp"
#include "audiomidi/MonitorInputAdapter.hpp"
#include "audiomidi/SoundPlayer.hpp"
#include "audiomidi/SoundRecorder.hpp"

#include "engine/Voice.hpp"
#include "engine/VoiceUtil.hpp"
#include "engine/FaderControl.hpp"
#include "engine/audio/mixer/MixerControls.hpp"

#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"

#include "engine/audio/mixer/AudioMixer.hpp"
#include "engine/audio/mixer/MixerControlsFactory.hpp"
#include "engine/audio/mixer/MainMixControls.hpp"

#include "engine/audio/server/CompoundAudioClient.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "engine/audio/server/RealTimeAudioServer.hpp"

#include "engine/control/CompoundControl.hpp"
#include "engine/control/BooleanControl.hpp"

#include "sequencer/FrameSeq.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
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

AudioMidiServices::AudioMidiServices(mpc::Mpc &mpcToUse) : mpc(mpcToUse)
{
    VoiceUtil::freqTable();
}

AudioMidiServices::~AudioMidiServices()
{
    offlineServer->setSharedPtr(nullptr);
}

void AudioMidiServices::start()
{
    server = std::make_shared<RealTimeAudioServer>();
    offlineServer = std::make_shared<NonRealTimeAudioServer>(server);
    soundRecorder = std::make_shared<SoundRecorder>(mpc);
    soundPlayer = std::make_shared<SoundPlayer>();

    setupMixer();

    createSynth();

    inputProcess = server->openAudioInput("RECORD IN");
    monitorInputAdapter =
        std::make_shared<MonitorInputAdapter>(mpc, inputProcess);

    const std::vector<std::string> outputNames{
        "STEREO OUT", "ASSIGNABLE MIX OUT 1/2", "ASSIGNABLE MIX OUT 3/4",
        "ASSIGNABLE MIX OUT 5/6", "ASSIGNABLE MIX OUT 7/8"};

    for (int i = 0; i < 5; i++)
    {
        outputProcesses.push_back(server->openAudioOutput(outputNames[i]));
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

    cac = std::make_shared<CompoundAudioClient>();
    cac->add(mpc.getSequencer()->getFrameSequencer().get());
    cac->add(mixer.get());

    mixer->getStrip("66")->setInputProcess(monitorInputAdapter);

    offlineServer->setSharedPtr(offlineServer);
    offlineServer->setClient(cac);

    offlineServer->start();
}

void AudioMidiServices::setMonitorLevel(const int level) const
{
    const auto sc = mixer->getMixerControls()->getStripControls("66");
    const auto mmc =
        std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
    std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))
        ->setValue(static_cast<float>(level));
}

void AudioMidiServices::muteMonitor(const bool mute) const
{
    const auto sc = mixer->getMixerControls()->getStripControls("66");
    const auto mmc =
        std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
    const auto mc =
        std::dynamic_pointer_cast<BooleanControl>(mmc->find("Mute"));
    mc->setValue(mute);
}

std::shared_ptr<SoundRecorder> AudioMidiServices::getSoundRecorder()
{
    return soundRecorder;
}

std::shared_ptr<SoundPlayer> AudioMidiServices::getSoundPlayer()
{
    return soundPlayer;
}

NonRealTimeAudioServer *AudioMidiServices::getAudioServer() const
{
    return offlineServer.get();
}

void AudioMidiServices::setupMixer()
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
    mixer = std::make_shared<AudioMixer>(mixerControls, offlineServer);
    muteMonitor(true);
    setAssignableMixOutLevels();
}

void AudioMidiServices::setMainLevel(const int i) const
{
    const auto sc = mixer->getMixerControls()->getStripControls("L-R");
    const auto cc =
        std::dynamic_pointer_cast<CompoundControl>(sc->find("Main"));
    std::dynamic_pointer_cast<FaderControl>(cc->find("Level"))->setValue(i);
}

int AudioMidiServices::getMainLevel() const
{
    const auto sc = mixer->getMixerControls()->getStripControls("L-R");
    const auto cc =
        std::dynamic_pointer_cast<CompoundControl>(sc->find("Main"));
    const auto val =
        std::dynamic_pointer_cast<FaderControl>(cc->find("Level"))->getValue();
    return static_cast<int>(val);
}

void AudioMidiServices::setRecordLevel(const int i) const
{
    soundRecorder->setInputGain(i);
    setMonitorLevel(i);
}

int AudioMidiServices::getRecordLevel() const
{
    return soundRecorder->getInputGain();
    /*
    // This is how we can get monitor output level
    auto sc = mixer->getMixerControls().lock()->getStripControls("66").lock();
    auto mmc =
    std::dynamic_pointer_cast<MainMixControls>(sc->find("Main").lock()); return
    static_cast<int>(std::dynamic_pointer_cast<FaderControl>(mmc->find("Level").lock())->getValue());
    */
}

void AudioMidiServices::setAssignableMixOutLevels() const
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

PreviewSoundPlayer &AudioMidiServices::getPreviewSoundPlayer() const
{
    return *basicSoundPlayerChannel.get();
}

void AudioMidiServices::createSynth()
{
    basicVoice = std::make_shared<Voice>(65, true);

    for (int i = 0; i < 32; i++)
    {
        voices.emplace_back(std::make_shared<Voice>(i + 1, false));
    }

    basicSoundPlayerChannel = std::make_unique<PreviewSoundPlayer>(
        mpc.getSampler(), mixer, basicVoice);
}

void AudioMidiServices::connectVoices()
{
    for (auto j = 0; j < 32; j++)
    {
        const auto ams1 = mixer->getStrip(std::to_string(j + 1));
        auto voice = voices[j];
        ams1->setInputProcess(voice);
        mixerConnections.emplace_back(
            new MixerInterconnection("con" + std::to_string(j), server.get()));
        const auto &mi = mixerConnections.back();
        ams1->setDirectOutputProcess(mi->getInputProcess());
        const auto ams2 = mixer->getStrip(std::to_string(j + 1 + 32));
        ams2->setInputProcess(mi->getOutputProcess());
    }

    basicSoundPlayerChannel->connectVoice();
}

void AudioMidiServices::initializeDiskRecorders()
{
    for (int i = 0; i < outputProcesses.size(); i++)
    {
        auto diskRecorder =
            std::make_shared<DiskRecorder>(mpc, outputProcesses[i], i);

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

void AudioMidiServices::destroyServices() const
{
    offlineServer->stop();
    closeIO();
    mixer->close();
    offlineServer->close();
}

void AudioMidiServices::closeIO() const
{
    server->closeAudioInput(inputProcess);

    for (auto &outputProcess : outputProcesses)
    {
        server->closeAudioOutput(outputProcess);
    }
}

bool AudioMidiServices::prepareBouncing(const DirectToDiskSettings *settings)
{
    const auto destinationDirectory =
        mpc.paths->recordingsPath() / settings->recordingName;

    fs::create_directory(destinationDirectory);

    for (int i = 0; i < diskRecorders.size(); i++)
    {
        const auto eapa = diskRecorders[i];

        if (!eapa->prepare(settings->lengthInFrames, settings->sampleRate,
                           !settings->splitStereoIntoLeftAndRightChannel,
                           destinationDirectory))
        {
            return false;
        }
    }

    bouncePrepared = true;
    return true;
}

void AudioMidiServices::startBouncing()
{
    if (!bouncePrepared)
    {
        return;
    }

    bouncePrepared = false;
    bouncing.store(true);
}

void AudioMidiServices::stopBouncing()
{
    if (!bouncing.load())
    {
        return;
    }

    mpc.getLayeredScreen()->openScreenById(
        ScreenId::VmpcRecordingFinishedScreen);
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
}

void AudioMidiServices::stopBouncingEarly()
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

void AudioMidiServices::startRecordingSound()
{
    recordingSound.store(true);
}

void AudioMidiServices::stopSoundRecorder(const bool cancel)
{
    if (cancel)
    {
        soundRecorder->cancel();
    }

    recordingSound.store(false);
}

bool AudioMidiServices::isBouncePrepared() const
{
    return bouncePrepared;
}

bool AudioMidiServices::isBouncing() const
{
    return bouncing.load();
}

bool AudioMidiServices::isRecordingSound() const
{
    return recordingSound.load();
}

// Should be called from the audio thread only!
void AudioMidiServices::changeSoundRecorderStateIfRequired()
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
void AudioMidiServices::changeBounceStateIfRequired()
{
    const auto directToDiskRecorderScreen =
        mpc.screens->get<ScreenId::VmpcDirectToDiskRecorderScreen>();

    if (isBouncing() && !wasBouncing)
    {

        wasBouncing = true;

        if (directToDiskRecorderScreen->isOffline())
        {
            //            std::vector<int> rates{ 44100, 48000, 88200 };
            //            auto rate =
            //            rates[static_cast<size_t>(directToDiskRecorderScreen->getSampleRate())];
            //            frameSeq->setSampleRate(offlineServer->getSampleRate());
            mpc.getSequencer()->getFrameSequencer()->start();

            if (getAudioServer()->isRealTime())
            {
                getAudioServer()->setRealTime(false);
            }

            //            if (server->getSampleRate() != rate)
            //            {
            //                server->setSampleRate(rate);
            //            }
        }
        else if (directToDiskRecorderScreen->getRecord() != 4)
        {
            mpc.getSequencer()->getFrameSequencer()->start();
        }

        for (const auto &diskRecorder : diskRecorders)
        {
            diskRecorder->start();
        }
    }
    else if (!isBouncing() && wasBouncing)
    {
        wasBouncing = false;

        if (directToDiskRecorderScreen->isOffline())
        {
            if (!getAudioServer()->isRealTime())
            {
                //                server->setSampleRate(static_cast<int>(getAudioServer()->getSampleRate()));
                getAudioServer()->setRealTime(true);
            }
        }
    }
}

// Should be called from the audio thread only!
void AudioMidiServices::switchMidiControlMappingIfRequired() const
{
    const auto vmpcMidiScreen = mpc.screens->get<ScreenId::VmpcMidiScreen>();

    if (vmpcMidiScreen->shouldSwitch.load())
    {
        vmpcMidiScreen->activePreset = vmpcMidiScreen->switchToPreset;

        vmpcMidiScreen->shouldSwitch.store(false);

        if (mpc.getLayeredScreen()->getCurrentScreenName() == "vmpc-midi")
        {
            mpc.getScreen()->open();
        }
    }
}

void AudioMidiServices::setMixerMasterLevel(const int8_t dbValue) const
{
    for (auto &v : voices)
    {
        v->setMasterLevel(dbValue);
    }
}

std::shared_ptr<AudioMixer> AudioMidiServices::getMixer()
{
    return mixer;
}

std::vector<std::shared_ptr<engine::Voice>> &AudioMidiServices::getVoices()
{
    return voices;
}

std::vector<mpc::engine::MixerInterconnection *> &
AudioMidiServices::getMixerConnections()
{
    return mixerConnections;
}
