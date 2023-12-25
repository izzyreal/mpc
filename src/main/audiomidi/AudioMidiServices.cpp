#include "AudioMidiServices.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

#include <Mpc.hpp>
#include <audiomidi/DirectToDiskSettings.hpp>
#include <audiomidi/DiskRecorder.hpp>
#include <audiomidi/MonitorInputAdapter.hpp>
#include <audiomidi/SoundPlayer.hpp>
#include <audiomidi/SoundRecorder.hpp>

#include "engine/Voice.hpp"
#include "engine/FaderControl.hpp"
#include <engine/audio/mixer/MixerControls.hpp>
#include "engine/Drum.hpp"

#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>

#include <engine/audio/mixer/AudioMixer.hpp>
#include <engine/audio/mixer/AudioMixerBus.hpp>
#include <engine/audio/mixer/MixerControlsFactory.hpp>
#include <engine/audio/mixer/MainMixControls.hpp>

#include <engine/audio/server/CompoundAudioClient.hpp>
#include <engine/audio/server/NonRealTimeAudioServer.hpp>
#include <engine/audio/server/RealTimeAudioServer.hpp>

#include <engine/control/CompoundControl.hpp>
#include <engine/control/BooleanControl.hpp>

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

AudioMidiServices::AudioMidiServices(mpc::Mpc& mpcToUse)
	: mpc(mpcToUse)
{
	Voice::freqTable();
    frameSeq = std::make_shared<mpc::sequencer::FrameSeq>(mpcToUse);
}

AudioMidiServices::~AudioMidiServices()
{
    offlineServer->setSharedPtr(nullptr);
}

void AudioMidiServices::start() {

	server = std::make_shared<RealTimeAudioServer>();
	offlineServer = std::make_shared<NonRealTimeAudioServer>(server);

    frameSeq->setSampleRate(offlineServer->getSampleRate());

	soundRecorder = std::make_shared<SoundRecorder>(mpc);
	soundPlayer = std::make_shared<SoundPlayer>();

	setupMixer();

    createSynth();

    inputProcess = server->openAudioInput("RECORD IN");
    monitorInputAdapter = std::make_shared<MonitorInputAdapter>(mpc, inputProcess);

    const std::vector<std::string> outputNames {
        "STEREO OUT",
        "ASSIGNABLE MIX OUT 1/2",
        "ASSIGNABLE MIX OUT 3/4",
        "ASSIGNABLE MIX OUT 5/6",
        "ASSIGNABLE MIX OUT 7/8"
    };

    for (int i = 0; i < 5; i++)
	{
		outputProcesses.push_back(server->openAudioOutput(outputNames[i]));
	}

	connectVoices();

	initializeDiskRecorders();

	mixer->getStrip(std::string("66"))->setDirectOutputProcess(soundRecorder);
	mixer->getStrip(std::string("67"))->setInputProcess(soundPlayer);
	auto sc = mixer->getMixerControls()->getStripControls("67");
	auto mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
	std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))->setValue(static_cast<float>(100));

	cac = std::make_shared<CompoundAudioClient>();
	cac->add(frameSeq.get());
	cac->add(mixer.get());

	mixer->getStrip("66")->setInputProcess(monitorInputAdapter);

	offlineServer->setSharedPtr(offlineServer);
	offlineServer->setClient(cac);

	offlineServer->start();
}

void AudioMidiServices::setMonitorLevel(int level)
{
	auto sc = mixer->getMixerControls()->getStripControls("66");
	auto mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
	std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))->setValue(static_cast<float>(level));
}

void AudioMidiServices::muteMonitor(bool mute)
{
	auto sc = mixer->getMixerControls()->getStripControls("66");
	auto mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
	auto mc = std::dynamic_pointer_cast<BooleanControl>(mmc->find("Mute"));
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

NonRealTimeAudioServer* AudioMidiServices::getAudioServer() {
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
	MixerControlsFactory::createBusStrips(std::dynamic_pointer_cast<MixerControls>(mixerControls), "L-R");

	/*
	* There are 32 voices. Each voice has one channel for mixing to STEREO OUT L/R, and one channel for mixing to an ASSIGNABLE MIX OUT. These are strips 1-64.
	* There's one channel for the PreviewSoundPlayer, which plays the metronome, preview and playX sounds. This is strip 65.
	* Finally there's one channel to receive and monitor sampler input, this is strip 66, and one for playing quick previews, on strip 67.
	* Hence nMixerChans = 67
	*/
	int nMixerChans = 67;
	MixerControlsFactory::createChannelStrips(mixerControls, nMixerChans);
	mixer = std::make_shared<AudioMixer>(mixerControls, offlineServer);
	muteMonitor(true);
	setAssignableMixOutLevels();
}

void AudioMidiServices::setMainLevel(int i)
{
	auto sc = mixer->getMixerControls()->getStripControls("L-R");
	auto cc = std::dynamic_pointer_cast<CompoundControl>(sc->find("Main"));
	std::dynamic_pointer_cast<FaderControl>(cc->find("Level"))->setValue(i);
}

int AudioMidiServices::getMainLevel() {
	auto sc = mixer->getMixerControls()->getStripControls("L-R");
	auto cc = std::dynamic_pointer_cast<CompoundControl>(sc->find("Main"));
	auto val = std::dynamic_pointer_cast<FaderControl>(cc->find("Level"))->getValue();
	return (int)(val);
}

void AudioMidiServices::setRecordLevel(int i) {
	soundRecorder->setInputGain(i);
	setMonitorLevel(i);
}

int AudioMidiServices::getRecordLevel() {
	return soundRecorder->getInputGain();
	/*
	// This is how we can get monitor output level
	auto sc = mixer->getMixerControls().lock()->getStripControls("66").lock();
	auto mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main").lock());
	return static_cast<int>(std::dynamic_pointer_cast<FaderControl>(mmc->find("Level").lock())->getValue());
	*/
}

void AudioMidiServices::setAssignableMixOutLevels()
{
	/*
	* We have to make sure the ASSIGNABLE MIX OUTs are audible. They're fixed at value 100.
	*/
	for (auto j = 1; j <= 4; j++) {
		std::string name = "AUX#" + std::to_string(j);
		auto sc = mixer->getMixerControls()->getStripControls(name);
		auto cc = std::dynamic_pointer_cast<CompoundControl>(sc->find(name));
		std::dynamic_pointer_cast<FaderControl>(cc->find("Level"))->setValue(100);
	}
}

Drum& AudioMidiServices::getDrum(int i)
{
	return soundPlayerChannels[i];
}

PreviewSoundPlayer& AudioMidiServices::getBasicPlayer()
{
    return *basicSoundPlayerChannel.get();
}

void AudioMidiServices::createSynth()
{
	basicVoice = std::make_shared<Voice>(65, true);

    for (int i = 0; i < 32; i ++)
    {
        voices.emplace_back(std::make_shared<Voice>(i + 1, false));
    }

    for (int i = 0; i < 4; i++)
    {
        auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");

        soundPlayerChannels.emplace_back(Drum(mpc.getSampler(),
                                         i,
                                         mixer,
                                         mixerSetupScreen.get(),
                                         voices,
                                         mixerConnections));
    }

    basicSoundPlayerChannel = std::make_unique<PreviewSoundPlayer>(mpc.getSampler(), mixer, basicVoice);
}

void AudioMidiServices::connectVoices()
{
    for (auto j = 0; j < 32; j++)
    {
        auto ams1 = mixer->getStrip(std::to_string(j + 1));
        auto voice = voices[j];
        ams1->setInputProcess(voice);
        mixerConnections.emplace_back(new MixerInterconnection("con" + std::to_string(j), server.get()));
        auto& mi = mixerConnections.back();
        ams1->setDirectOutputProcess(mi->getInputProcess());
        auto ams2 = mixer->getStrip(std::to_string(j + 1 + 32));
        ams2->setInputProcess(mi->getOutputProcess());
    }

    basicSoundPlayerChannel->connectVoice();
}

void AudioMidiServices::initializeDiskRecorders()
{
    for (int i = 0; i < outputProcesses.size(); i++)
    {
        auto diskRecorder = std::make_shared<DiskRecorder>(mpc, outputProcesses[i], i);

        diskRecorders.push_back(diskRecorder);

        if (i == 0)
        {
            mixer->getMainStrip()->setDirectOutputProcess(diskRecorders.back());
        }
        else
        {
            mixer->getStrip(std::string("AUX#" + std::to_string(i)))->setDirectOutputProcess(diskRecorders.back());
        }
    }
}

void AudioMidiServices::destroyServices()
{
	offlineServer->stop();
	closeIO();
	mixer->close();
	offlineServer->close();
}

void AudioMidiServices::closeIO()
{
    server->closeAudioInput(inputProcess);

	for (auto & outputProcess : outputProcesses)
	{
		server->closeAudioOutput(outputProcess);
	}
}

bool AudioMidiServices::prepareBouncing(DirectToDiskSettings* settings)
{
    const auto destinationDirectory = mpc.paths->recordingsPath() / settings->recordingName;

    fs::create_directory(destinationDirectory);

    for (int i = 0; i < diskRecorders.size(); i++)
	{
		auto eapa = diskRecorders[i];

		if (!eapa->prepare(
                settings->lengthInFrames,
                settings->sampleRate,
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

	mpc.getLayeredScreen()->openScreen("vmpc-recording-finished");
	bouncing.store(false);

    auto directToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");

    if (directToDiskRecorderScreen->loopWasEnabled)
    {
        auto seq = mpc.getSequencer()->getSequence(directToDiskRecorderScreen->sq);
        seq->setLoopEnabled(true);
        directToDiskRecorderScreen->loopWasEnabled = false;
    }
}

void AudioMidiServices::stopBouncingEarly()
{
	if (!bouncing.load())
    {
        return;
    }

	for (auto& recorder : diskRecorders)
    {
        recorder->stopEarly();
    }

    stopBouncing();
}

void AudioMidiServices::startRecordingSound()
{
	recordingSound.store(true);
}

void AudioMidiServices::stopSoundRecorder(bool cancel)
{
	if (cancel)
		soundRecorder->cancel();

	recordingSound.store(false);
}

std::shared_ptr<mpc::sequencer::FrameSeq> AudioMidiServices::getFrameSequencer()
{
	return frameSeq;
}

bool AudioMidiServices::isBouncePrepared()
{
	return bouncePrepared;
}

const bool AudioMidiServices::isBouncing()
{
	return bouncing.load();
}

const bool AudioMidiServices::isRecordingSound() {
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
    auto directToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");

    if (isBouncing() && !wasBouncing) {

        wasBouncing = true;

        if (directToDiskRecorderScreen->isOffline())
        {
//            std::vector<int> rates{ 44100, 48000, 88200 };
//            auto rate = rates[static_cast<size_t>(directToDiskRecorderScreen->getSampleRate())];
//            frameSeq->setSampleRate(offlineServer->getSampleRate());
            frameSeq->start();

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
            frameSeq->start();
        }

        for (auto& diskRecorder : diskRecorders)
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
void AudioMidiServices::switchMidiControlMappingIfRequired()
{
    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");

    if (vmpcMidiScreen->shouldSwitch.load())
    {
        vmpcMidiScreen->activePreset = vmpcMidiScreen->switchToPreset;

        vmpcMidiScreen->shouldSwitch.store(false);

        if (mpc.getLayeredScreen()->getCurrentScreenName() == "vmpc-midi")
        {
            mpc.getActiveControls()->open();
        }
    }
}

void AudioMidiServices::setMixerMasterLevel(int8_t dbValue)
{
    for(auto& v : voices)
    {
        v->setMasterLevel(dbValue);
    }
}
