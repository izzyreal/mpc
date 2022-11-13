#include "AudioMidiServices.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

// mpc
#include <Paths.hpp>
#include <Mpc.hpp>
#include <audiomidi/DirectToDiskSettings.hpp>
#include <audiomidi/DiskRecorder.hpp>
#include <audiomidi/SoundRecorder.hpp>
#include <audiomidi/SoundPlayer.hpp>
#include <audiomidi/MonitorInputAdapter.hpp>
#include <audiomidi/MpcMidiOutput.hpp>

#include <mpc/MpcVoice.hpp>
#include <mpc/MpcBasicSoundPlayerChannel.hpp>
#include <mpc/MpcBasicSoundPlayerControls.hpp>
#include <mpc/MpcFaderControl.hpp>
#include <mpc/MpcMixerControls.hpp>
#include <mpc/MpcMultiMidiSynth.hpp>
#include <mpc/MpcMultiSynthControls.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcSoundPlayerControls.hpp>

#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>

// ctoot
#include <audio/core/ChannelFormat.hpp>

#include <audio/mixer/AudioMixer.hpp>
#include <audio/mixer/AudioMixerBus.hpp>
#include <audio/mixer/AudioMixerStrip.hpp>
#include <audio/mixer/MixerControlsFactory.hpp>
#include <audio/mixer/MixerControls.hpp>
#include <audio/mixer/MainMixControls.hpp>

#include <audio/server/CompoundAudioClient.hpp>
#include <audio/server/IOAudioProcess.hpp>
#include <audio/server/NonRealTimeAudioServer.hpp>
#include <audio/server/ExternalAudioServer.hpp>

#include <audio/system/MixerConnectedAudioSystem.hpp>
#include <audio/system/AudioOutput.hpp>

#include <control/CompoundControl.hpp>

#include <midi/core/DefaultConnectedMidiSystem.hpp>

#include <synth/SynthChannel.hpp>
#include <synth/SynthRack.hpp>
#include <synth/SynthRackControls.hpp>

#include <audio/reverb/BarrControls.hpp>

#include <audio/core/AudioServices.hpp>
#include <audio/spi/AudioServiceProvider.hpp>

#include <synth/SynthServices.hpp>
#include <synth/SynthChannelServices.hpp>

#include <mpc/MpcSampler.hpp>

// moduru
#include <file/FileUtil.hpp>

// stl
#include <cmath>
#include <string>

using namespace mpc;
using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;

using namespace ctoot::audio::server;
using namespace ctoot::audio::core;
using namespace ctoot::audio::mixer;

AudioMidiServices::AudioMidiServices(mpc::Mpc& mpc)
	: mpc(mpc)
{
	frameSeq = std::make_shared<mpc::sequencer::FrameSeq>(mpc);
	AudioServices::scan();
	ctoot::synth::SynthServices::scan();
	ctoot::synth::SynthChannelServices::scan();
    ctoot::synth::SynthChannel::freqTable();
}

void AudioMidiServices::start(const int inputCount, const int outputCount) {

	midiSystem = std::make_shared<ctoot::midi::core::DefaultConnectedMidiSystem>();

	server = std::make_shared<ExternalAudioServer>();
	offlineServer = std::make_shared<NonRealTimeAudioServer>(server);

	soundRecorder = std::make_shared<SoundRecorder>(mpc);
	soundPlayer = std::make_shared<SoundPlayer>();

	setupMixer();

	inputProcesses = std::vector<std::shared_ptr<IOAudioProcess>>(inputCount <= 1 ? inputCount : 1);
	outputProcesses = std::vector<IOAudioProcess*>(outputCount <= 5 ? outputCount : 5);

	for (auto& p : inputProcesses)
	{
		p = nullptr;
	}

	for (auto& p : outputProcesses)
	{
		p = nullptr;
	}

	for (int i = 0; i < inputProcesses.size(); i++)
	{
		inputProcesses[i] = std::shared_ptr<IOAudioProcess>(server->openAudioInput(getInputNames()[i], "mpc_in" + std::to_string(i)));
		// For now we assume there is only 1 stereo input pair max
		if (i == 0)
			monitorInputAdapter = std::make_shared<MonitorInputAdapter>(mpc, inputProcesses[i].get());
	}

	for (int i = 0; i < outputProcesses.size(); i++)
	{
		outputProcesses[i] = server->openAudioOutput(getOutputNames()[i], "mpc_out" + std::to_string(i));
	}

	createSynth();
	connectVoices();

    mpcMidiOutput = std::make_shared<MpcMidiOutput>();

	initializeDiskRecorders();

	mixer->getStrip(std::string("66")).lock()->setDirectOutputProcess(soundRecorder);
	mixer->getStrip(std::string("67")).lock()->setInputProcess(soundPlayer);
	auto sc = mixer->getMixerControls().lock()->getStripControls("67").lock();
	auto mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main").lock());
	std::dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level").lock())->setValue(static_cast<float>(100));

	cac = std::make_shared<CompoundAudioClient>();
	cac->add(frameSeq.get());
	cac->add(mixer.get());

	mixer->getStrip("66").lock()->setInputProcess(monitorInputAdapter);

	offlineServer->setWeakPtr(offlineServer);
	offlineServer->setClient(cac);

	offlineServer->start();
}

void AudioMidiServices::setPreviewClickVolume(int volume)
{
	auto sc = mixer->getMixerControls().lock()->getStripControls("65").lock();
	auto mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main").lock());
	std::dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level").lock())->setValue(static_cast<float>(volume));
}

void AudioMidiServices::setMonitorLevel(int level)
{
	auto sc = mixer->getMixerControls().lock()->getStripControls("66").lock();
	auto mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main").lock());
	std::dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level").lock())->setValue(static_cast<float>(level));
}

void AudioMidiServices::muteMonitor(bool mute)
{
	auto sc = mixer->getMixerControls().lock()->getStripControls("66").lock();
	auto mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main").lock());
	auto controlRow = std::dynamic_pointer_cast<CompoundControl>(mmc->find("ControlRow").lock());
	auto mc = std::dynamic_pointer_cast<BooleanControl>(controlRow->find("Mute").lock());
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
	mixerControls = std::make_shared<ctoot::mpc::MpcMixerControls>("MpcMixerControls", 1.f);

	// AUX#1 - #4 represent ASSIGNABLE MIX OUT 1/2, 3/4, 5/6 and 7/8
	mixerControls->createAuxBusControls("AUX#1", ChannelFormat::STEREO());
	mixerControls->createAuxBusControls("AUX#2", ChannelFormat::STEREO());
	mixerControls->createAuxBusControls("AUX#3", ChannelFormat::STEREO());
	mixerControls->createAuxBusControls("AUX#4", ChannelFormat::STEREO());

	// FX#1 Represents the MPC2000XL's only FX send bus
	mixerControls->createFxBusControls("FX#1", ChannelFormat::STEREO());
	int nReturns = 1;

	// L/R represents STEREO OUT L/R
	ctoot::audio::mixer::MixerControlsFactory::createBusStrips(std::dynamic_pointer_cast<ctoot::audio::mixer::MixerControls>(mixerControls), "L-R", ChannelFormat::STEREO(), nReturns);

	/*
	* There are 32 voices. Each voice has one channel for mixing to STEREO OUT L/R, and one channel for mixing to an ASSIGNABLE MIX OUT. These are strips 1-64.
	* There's one channel for the MpcBasicSoundPlayerChannel, which plays the metronome, preview and playX sounds. This is strip 65.
	* Finally there's one channel to receive and monitor sampler input, this is strip 66, and one for playing quick previews, on strip 67.
	* Hence nMixerChans = 67
	*/
	int nMixerChans = 67;
	ctoot::audio::mixer::MixerControlsFactory::createChannelStrips(mixerControls, nMixerChans);
	mixer = std::make_shared<ctoot::audio::mixer::AudioMixer>(mixerControls, offlineServer);
	audioSystem = std::make_shared<ctoot::audio::system::MixerConnectedAudioSystem>(mixer);
	audioSystem->setAutoConnect(false);
	muteMonitor(true);
	setAssignableMixOutLevels();
}

void AudioMidiServices::setMasterLevel(int i)
{
	auto sc = mixer->getMixerControls().lock()->getStripControls("L-R").lock();
	auto cc = std::dynamic_pointer_cast<ctoot::control::CompoundControl>(sc->find("Main").lock());
	std::dynamic_pointer_cast<ctoot::mpc::MpcFaderControl>(cc->find("Level").lock())->setValue(i);
}

int AudioMidiServices::getMasterLevel() {
	auto sc = mixer->getMixerControls().lock()->getStripControls("L-R").lock();
	auto cc = std::dynamic_pointer_cast<ctoot::control::CompoundControl>(sc->find("Main").lock());
	auto val = std::dynamic_pointer_cast<ctoot::mpc::MpcFaderControl>(cc->find("Level").lock())->getValue();
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
	auto mmc = std::dynamic_pointer_cast<ctoot::audio::mixer::MainMixControls>(sc->find("Main").lock());
	return static_cast<int>(std::dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level").lock())->getValue());
	*/
}

void AudioMidiServices::setAssignableMixOutLevels()
{
	/*
	* We have to make sure the ASSIGNABLE MIX OUTs are audible. They're fixed at value 100.
	*/
	for (auto j = 1; j <= 4; j++) {
		std::string name = "AUX#" + std::to_string(j);
		auto sc = mixer->getMixerControls().lock()->getStripControls(name).lock();
		auto cc = std::dynamic_pointer_cast<ctoot::control::CompoundControl>(sc->find(name).lock());
		std::dynamic_pointer_cast<ctoot::mpc::MpcFaderControl>(cc->find("Level").lock())->setValue(100);
	}
}

std::vector<std::string> AudioMidiServices::getInputNames()
{
	if (!server) return std::vector<std::string>{"<disabled>"};
	return server->getAvailableInputNames();
}

std::vector<std::string> AudioMidiServices::getOutputNames()
{
	if (!server) return std::vector<std::string>{"<disabled>"};
	return server->getAvailableOutputNames();
}

std::shared_ptr<ctoot::mpc::MpcMultiMidiSynth> AudioMidiServices::getMms()
{
	return mms;
}

void AudioMidiServices::createSynth()
{
	synthRackControls = std::make_shared<ctoot::synth::SynthRackControls>(1);
	synthRack = std::make_shared<ctoot::synth::SynthRack>(synthRackControls, midiSystem, audioSystem);
	msc = std::make_shared<ctoot::mpc::MpcMultiSynthControls>();
	synthRackControls->setSynthControls(0, msc);
	mms = std::dynamic_pointer_cast<ctoot::mpc::MpcMultiMidiSynth>(synthRack->getMidiSynth(0).lock());

	auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");

	for (int i = 0; i < 4; i++)
	{
		auto m = std::make_shared<ctoot::mpc::MpcSoundPlayerControls>(mms, std::dynamic_pointer_cast<ctoot::mpc::MpcSampler>(mpc.getSampler()), i, mixer, server, mixerSetupScreen.get());
		msc->setChannelControls(i, m);
		synthChannelControls.push_back(m);
	}

	basicVoice = std::make_shared<ctoot::mpc::MpcVoice>(65, true);
	auto m = std::make_shared<ctoot::mpc::MpcBasicSoundPlayerControls>(mpc.getSampler(), mixer, basicVoice);
	msc->setChannelControls(4, m);
	synthChannelControls.push_back(std::move(m));
}

void AudioMidiServices::connectVoices()
{
    mpc.getDrum(0)->connectVoices();
	mpc.getBasicPlayer()->connectVoice();
}

std::shared_ptr<MpcMidiOutput> AudioMidiServices::getMidiPorts()
{
	return mpcMidiOutput;
}

void AudioMidiServices::initializeDiskRecorders()
{
	for (int i = 0; i < outputProcesses.size(); i++) {
		auto diskRecorder = std::make_shared<DiskRecorder>(outputProcesses[i], "diskwriter" + std::to_string(i));

        diskRecorders.push_back(diskRecorder);

        if (i == 0)
			mixer->getMainStrip().lock()->setDirectOutputProcess(diskRecorders.back());
		else
			mixer->getStrip(std::string("AUX#" + std::to_string(i))).lock()->setDirectOutputProcess(diskRecorders.back());
	}
}

void AudioMidiServices::destroyServices()
{
	offlineServer->stop();
	closeIO();
	audioSystem->close();
	destroySynth();
	mixer->close();
	offlineServer->close();
	midiSystem->close();
}

void AudioMidiServices::destroySynth()
{
	synthRack->close();
}

void AudioMidiServices::closeIO()
{
	for (auto j = 0; j < inputProcesses.size(); j++)
	{
		if (inputProcesses[j])
			server->closeAudioInput(inputProcesses[j].get());
	}

	for (auto j = 0; j < outputProcesses.size(); j++)
	{
		if (outputProcesses[j] == nullptr)
			continue;

		server->closeAudioOutput(outputProcesses[j]);
	}
}

bool AudioMidiServices::prepareBouncing(DirectToDiskSettings* settings)
{
	auto indivFileNames = std::vector<std::string>{ "L-R.wav", "1-2.wav", "3-4.wav", "5-6.wav", "7-8.wav" };
	std::string sep = moduru::file::FileUtil::getSeparator();

	for (int i = 0; i < diskRecorders.size(); i++)
	{
		auto eapa = diskRecorders[i];
		auto absolutePath = mpc::Paths::recordingsPath() + indivFileNames[i];

		if (!eapa->prepare(absolutePath, settings->lengthInFrames, settings->sampleRate))
			return false;
	}

	bouncePrepared = true;
	return true;
}

bool AudioMidiServices::startBouncing()
{
	if (!bouncePrepared)
		return false;

	bouncePrepared = false;
	bouncing.store(true);
	return true;
}

bool AudioMidiServices::stopBouncing()
{
	if (!bouncing.load())
		return false;

	mpc.getLayeredScreen()->openScreen("vmpc-recording-finished");
	bouncing.store(false);

    auto directToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");

    if (directToDiskRecorderScreen->loopWasEnabled)
    {
        auto seq = mpc.getSequencer()->getSequence(directToDiskRecorderScreen->sq);
        seq->setLoopEnabled(true);
        directToDiskRecorderScreen->loopWasEnabled = false;
    }

    return true;
}

bool AudioMidiServices::stopBouncingEarly()
{
	if (!bouncing.load())
		return false;

	for (auto& recorder : diskRecorders)
		recorder->stopEarly();

	mpc.getLayeredScreen()->openScreen("vmpc-recording-finished");
	bouncing.store(false);
	return true;
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
            std::vector<int> rates{ 44100, 48000, 88200 };
            auto rate = rates[static_cast<size_t>(directToDiskRecorderScreen->getSampleRate())];
            getFrameSequencer()->start(rate);

            if (getAudioServer()->isRealTime())
            {
                server->setSampleRate(rate);
                getAudioServer()->setRealTime(false);
            }
        }
        else if (directToDiskRecorderScreen->getRecord() != 4)
        {
            getFrameSequencer()->start(static_cast<int>(server->getSampleRate()));
        }

        for (auto& diskRecorder : diskRecorders)
            diskRecorder->start();
    }
    else if (!isBouncing() && wasBouncing)
    {
        wasBouncing = false;

        if (directToDiskRecorderScreen->isOffline())
        {
            if (!getAudioServer()->isRealTime())
            {
                server->setSampleRate(static_cast<int>(getAudioServer()->getSampleRate()));
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
        for (auto& c : vmpcMidiScreen->realtimeSwitchCommands)
        {
            vmpcMidiScreen->setLabelCommand(c.first, c.second);
        }

        vmpcMidiScreen->shouldSwitch.store(false);
    }
}
