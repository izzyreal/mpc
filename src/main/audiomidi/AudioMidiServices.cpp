#include "AudioMidiServices.hpp"

// mpc
#include <Paths.hpp>
#include <Mpc.hpp>
#include <audiomidi/DirectToDiskSettings.hpp>
#include <audiomidi/DiskRecorder.hpp>
#include <audiomidi/SoundRecorder.hpp>
#include <audiomidi/SoundPlayer.hpp>
#include <audiomidi/MonitorInputAdapter.hpp>
#include <audiomidi/MpcMidiPorts.hpp>

#include <nvram/NvRam.hpp>
#include <nvram/AudioMidiConfig.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Sequencer.hpp>

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
#include <lcdgui/Screens.hpp>

// ctoot
#include <audio/core/AudioFormat.hpp>
#include <audio/core/AudioControlsChain.hpp>
#include <audio/core/AudioProcess.hpp>
#include <audio/core/ChannelFormat.hpp>

#include <audio/mixer/AudioMixer.hpp>
#include <audio/mixer/AudioMixerBus.hpp>
#include <audio/mixer/AudioMixerStrip.hpp>
#include <audio/mixer/MixerControlsFactory.hpp>
#include <audio/mixer/MixerControls.hpp>
#include <audio/mixer/MainMixControls.hpp>

#include <audio/server/CompoundAudioClient.hpp>
#include <audio/server/IOAudioProcess.hpp>
#include <audio/server/AudioServer.hpp>
#include <audio/server/NonRealTimeAudioServer.hpp>
#include <audio/server/ExternalAudioServer.hpp>

#include <audio/system/MixerConnectedAudioSystem.hpp>
#include <audio/system/AudioDevice.hpp>
#include <audio/system/AudioOutput.hpp>

#include <control/CompoundControl.hpp>
#include <control/Control.hpp>

#include <midi/core/MidiSystem.hpp>
#include <midi/core/DefaultConnectedMidiSystem.hpp>

#include <synth/MidiChannel.hpp>
#include <synth/MidiSynth.hpp>
#include <synth/SynthChannel.hpp>
#include <synth/SynthChannelControls.hpp>
#include <synth/SynthRack.hpp>
#include <synth/SynthRackControls.hpp>

#include <audio/reverb/BarrControls.hpp>
#include <audio/reverb/BarrProcess.hpp>

#include <audio/core/AudioServices.hpp>
#include <audio/spi/AudioServiceProvider.hpp>

#include <synth/synths/multi/MultiSynthServiceProvider.hpp>
#include <synth/SynthServices.hpp>
#include <synth/SynthChannelServices.hpp>

// moduru
#include <file/File.hpp>
#include <file/FileUtil.hpp>
#include <lang/StrUtil.hpp>

#include <mpc/MpcSampler.hpp>
#include <mpc/MpcMixerSetupGui.hpp>

#include <Logger.hpp>

// stl
#include <cmath>
#include <string>

using namespace mpc;
using namespace mpc::audiomidi;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;

using namespace ctoot::audio::server;
using namespace ctoot::audio::core;
using namespace ctoot::audio::mixer;

using namespace std;

AudioMidiServices::AudioMidiServices(mpc::Mpc& mpc)
	: mpc(mpc)
{
	frameSeq = make_shared<mpc::sequencer::FrameSeq>();
	AudioServices::scan();
	ctoot::synth::SynthServices::scan();
	ctoot::synth::SynthChannelServices::scan();
}

void AudioMidiServices::start(const int sampleRate, const int inputCount, const int outputCount) {

	midiSystem = make_shared<ctoot::midi::core::DefaultConnectedMidiSystem>();

	server = make_shared<ExternalAudioServer>();
	offlineServer = make_shared<NonRealTimeAudioServer>(server);

	soundRecorder = make_shared<SoundRecorder>(mpc);
	soundPlayer = make_shared<SoundPlayer>();

	setupMixer();

	inputProcesses = vector<shared_ptr<IOAudioProcess>>(inputCount <= 1 ? inputCount : 1);
	outputProcesses = vector<IOAudioProcess*>(outputCount <= 5 ? outputCount : 5);

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
		inputProcesses[i] = shared_ptr<IOAudioProcess>(server->openAudioInput(getInputNames()[i], "mpc_in" + to_string(i)));
		// For now we assume there is only 1 stereo input pair max
		if (i == 0)
			monitorInputAdapter = make_shared<MonitorInputAdapter>(mpc, inputProcesses[i].get());
	}

	for (int i = 0; i < outputProcesses.size(); i++)
	{
		outputProcesses[i] = server->openAudioOutput(getOutputNames()[i], "mpc_out" + to_string(i));
	}

	createSynth();
	connectVoices();

	mpcMidiPorts = make_shared<MpcMidiPorts>();

	initializeDiskRecorders();

	mixer->getStrip(string("66")).lock()->setDirectOutputProcess(soundRecorder);
	mixer->getStrip(string("67")).lock()->setInputProcess(soundPlayer);
	auto sc = mixer->getMixerControls().lock()->getStripControls("67").lock();
	auto mmc = dynamic_pointer_cast<MainMixControls>(sc->find("Main").lock());
	dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level").lock())->setValue(static_cast<float>(100));

	cac = make_shared<CompoundAudioClient>();
	cac->add(frameSeq.get());
	cac->add(mixer.get());

	mixer->getStrip("66").lock()->setInputProcess(monitorInputAdapter);

	offlineServer->setWeakPtr(offlineServer);
	offlineServer->setClient(cac);

	offlineServer->start();
}

void AudioMidiServices::setMonitorLevel(int level) {
	auto sc = mixer->getMixerControls().lock()->getStripControls("66").lock();
	auto mmc = dynamic_pointer_cast<MainMixControls>(sc->find("Main").lock());
	dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level").lock())->setValue(static_cast<float>(level));
}

void AudioMidiServices::muteMonitor(bool mute)
{
	auto sc = mixer->getMixerControls().lock()->getStripControls("66").lock();
	auto mmc = dynamic_pointer_cast<MainMixControls>(sc->find("Main").lock());
	auto controlRow = dynamic_pointer_cast<CompoundControl>(mmc->find("ControlRow").lock());
	auto mc = dynamic_pointer_cast<BooleanControl>(controlRow->find("Mute").lock());
	mc->setValue(mute);
}

vector<weak_ptr<DiskRecorder>> AudioMidiServices::getDiskRecorders()
{
	vector <weak_ptr<DiskRecorder>> res;
	for (auto& diskRecorder : diskRecorders) {
		res.push_back(diskRecorder);
	}
	return res;
}

weak_ptr<SoundRecorder> AudioMidiServices::getSoundRecorder()
{
	return soundRecorder;
}

weak_ptr<SoundPlayer> AudioMidiServices::getSoundPlayer()
{
	return soundPlayer;
}

NonRealTimeAudioServer* AudioMidiServices::getAudioServer() {
	return offlineServer.get();
}

void AudioMidiServices::setupMixer()
{
	mixerControls = make_shared<ctoot::mpc::MpcMixerControls>("MpcMixerControls", 1.f);

	// AUX#1 - #4 represent ASSIGNABLE MIX OUT 1/2, 3/4, 5/6 and 7/8
	mixerControls->createAuxBusControls("AUX#1", ChannelFormat::STEREO());
	mixerControls->createAuxBusControls("AUX#2", ChannelFormat::STEREO());
	mixerControls->createAuxBusControls("AUX#3", ChannelFormat::STEREO());
	mixerControls->createAuxBusControls("AUX#4", ChannelFormat::STEREO());

	// FX#1 Represents the MPC2000XL's only FX send bus
	mixerControls->createFxBusControls("FX#1", ChannelFormat::STEREO());
	int nReturns = 1;

	// L/R represents STEREO OUT L/R
	ctoot::audio::mixer::MixerControlsFactory::createBusStrips(dynamic_pointer_cast<ctoot::audio::mixer::MixerControls>(mixerControls), "L-R", ChannelFormat::STEREO(), nReturns);

	/*
	* There are 32 voices. Each voice has one channel for mixing to STEREO OUT L/R, and one channel for mixing to an ASSIGNABLE MIX OUT. These are strips 1-64.
	* There's one channel for the MpcBasicSoundPlayerChannel, which plays the metronome, preview and playX sounds. This is strip 65.
	* Finally there's one channel to receive and monitor sampler input, this is strip 66, and one for playing quick previews, on strip 67.
	* Hence nMixerChans = 67
	*/
	int nMixerChans = 67;
	ctoot::audio::mixer::MixerControlsFactory::createChannelStrips(mixerControls, nMixerChans);
	mixer = make_shared<ctoot::audio::mixer::AudioMixer>(mixerControls, offlineServer);
	audioSystem = make_shared<ctoot::audio::system::MixerConnectedAudioSystem>(mixer);
	audioSystem->setAutoConnect(false);
	setMasterLevel(nvram::NvRam::getMasterLevel());
	setRecordLevel(nvram::NvRam::getRecordLevel());
	muteMonitor(true);
	setAssignableMixOutLevels();
}

void AudioMidiServices::setMasterLevel(int i)
{
	auto sc = mixer->getMixerControls().lock()->getStripControls("L-R").lock();
	auto cc = dynamic_pointer_cast<ctoot::control::CompoundControl>(sc->find("Main").lock());
	dynamic_pointer_cast<ctoot::mpc::MpcFaderControl>(cc->find("Level").lock())->setValue(i);
}

int AudioMidiServices::getMasterLevel() {
	auto sc = mixer->getMixerControls().lock()->getStripControls("L-R").lock();
	auto cc = dynamic_pointer_cast<ctoot::control::CompoundControl>(sc->find("Main").lock());
	auto val = dynamic_pointer_cast<ctoot::mpc::MpcFaderControl>(cc->find("Level").lock())->getValue();
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
	auto mmc = dynamic_pointer_cast<ctoot::audio::mixer::MainMixControls>(sc->find("Main").lock());
	return static_cast<int>(dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level").lock())->getValue());
	*/
}

void AudioMidiServices::setAssignableMixOutLevels()
{
	/*
	* We have to make sure the ASSIGNABLE MIX OUTs are audible. They're fixed at value 100.
	*/
	for (auto j = 1; j <= 4; j++) {
		string name = "AUX#" + to_string(j);
		auto sc = mixer->getMixerControls().lock()->getStripControls(name).lock();
		auto cc = dynamic_pointer_cast<ctoot::control::CompoundControl>(sc->find(name).lock());
		dynamic_pointer_cast<ctoot::mpc::MpcFaderControl>(cc->find("Level").lock())->setValue(100);
	}
}

vector<string> AudioMidiServices::getInputNames()
{
	if (!server) return vector<string>{"<disabled>"};
	return server->getAvailableInputNames();
}

vector<string> AudioMidiServices::getOutputNames()
{
	if (!server) return vector<string>{"<disabled>"};
	return server->getAvailableOutputNames();
}

weak_ptr<ctoot::mpc::MpcMultiMidiSynth> AudioMidiServices::getMms()
{
	return mms;
}

void AudioMidiServices::createSynth()
{
	synthRackControls = make_shared<ctoot::synth::SynthRackControls>(1);
	synthRack = make_shared<ctoot::synth::SynthRack>(synthRackControls, midiSystem, audioSystem);
	msc = make_shared<ctoot::mpc::MpcMultiSynthControls>();
	synthRackControls->setSynthControls(0, msc);
	mms = dynamic_pointer_cast<ctoot::mpc::MpcMultiMidiSynth>(synthRack->getMidiSynth(0).lock());

	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));

	for (int i = 0; i < 4; i++)
	{
		auto m = make_shared<ctoot::mpc::MpcSoundPlayerControls>(mms, dynamic_pointer_cast<ctoot::mpc::MpcSampler>(Mpc::instance().getSampler().lock()), i, mixer, server, mixerSetupScreen.get());
		msc->setChannelControls(i, m);
		synthChannelControls.push_back(m);
	}

	basicVoice = make_shared<ctoot::mpc::MpcVoice>(65, true);
	auto m = make_shared<ctoot::mpc::MpcBasicSoundPlayerControls>(Mpc::instance().getSampler(), mixer, basicVoice);
	msc->setChannelControls(4, m);
	synthChannelControls.push_back(std::move(m));
}

void AudioMidiServices::connectVoices()
{
	Mpc::instance().getDrums()[0]->connectVoices();
	Mpc::instance().getBasicPlayer()->connectVoice();
}

weak_ptr<MpcMidiPorts> AudioMidiServices::getMidiPorts()
{
	return mpcMidiPorts;
}

void AudioMidiServices::initializeDiskRecorders()
{
	for (int i = 0; i < outputProcesses.size(); i++) {
		auto diskRecorder = make_shared<DiskRecorder>(outputProcesses[i], "diskwriter" + to_string(i));
		if (i == 0) {
			mixer->getMainStrip().lock()->setDirectOutputProcess(diskRecorder);
		}
		else {
			mixer->getStrip(string("AUX#" + to_string(i))).lock()->setDirectOutputProcess(diskRecorder);
		}
		diskRecorders.push_back(std::move(diskRecorder));
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
		{
			server->closeAudioInput(inputProcesses[j].get());
		}
	}

	for (auto j = 0; j < outputProcesses.size(); j++)
	{
		if (outputProcesses[j] == nullptr)
		{
			continue;
		}
		server->closeAudioOutput(outputProcesses[j]);
	}
}

void AudioMidiServices::prepareBouncing(DirectToDiskSettings* settings)
{
	auto indivFileNames = vector<string>{ "L-R.wav", "1-2.wav", "3-4.wav", "5-6.wav", "7-8.wav" };
	string sep = moduru::file::FileUtil::getSeparator();

	for (int i = 0; i < diskRecorders.size(); i++)
	{
		auto eapa = diskRecorders[i];
		auto absolutePath = mpc::Paths::home() + sep + "vMPC" + sep + "recordings" + sep + indivFileNames[i];
		eapa->prepare(absolutePath, settings->lengthInFrames, settings->sampleRate);
	}

	bouncePrepared = true;
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

	Mpc::instance().getLayeredScreen().lock()->openScreen("vmpc-recording-finished");
	bouncing.store(false);
}

void AudioMidiServices::startRecordingSound()
{
	recordingSound.store(true);
}

void AudioMidiServices::stopSoundRecorder()
{
	recordingSound.store(false);
}

weak_ptr<mpc::sequencer::FrameSeq> AudioMidiServices::getFrameSequencer()
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
