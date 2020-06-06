#pragma once

#include <lcdgui/LayeredScreen.hpp>
#include <ui/Uis.hpp>

#include <disk/DiskController.hpp>
#include <disk/ProgramLoader.hpp>

#include <observer/Observable.hpp>

#include <Logger.hpp>

#include <vector>
#include <string>
#include <thread>
#include <memory>

namespace ctoot::mpc
{
	class MpcSoundPlayerChannel;
	class MpcBasicSoundPlayerChannel;
	class MpcMultiMidiSynth;
}

namespace mpc::ui
{
	class UserDefaults;
	class Uis;
}

namespace mpc::hardware
{
	class Hardware;
}

namespace mpc::disk
{
	class Stores;
	class AbstractDisk;
}

namespace mpc::controls
{
	class Controls;
	class BaseControls;
	class GlobalReleaseControls;
}

namespace mpc::lcdgui
{
	class LayeredScreen;
}

namespace mpc::audiomidi
{
	class EventHandler;
	class AudioMidiServices;
	class MpcMidiInput;
	class MpcMidiPorts;
}

namespace mpc::sequencer
{
	class Sequencer;
}

namespace mpc::sampler
{
	class Sampler;
}

namespace mpc {
	class Mpc
		: public moduru::observer::Observable
	{
	public:
		static Mpc& instance()
		{
			static auto mpc = Mpc();
			return mpc;
		}

	private:
		std::thread loadSoundThread{};
		std::unique_ptr<mpc::disk::ProgramLoader> programLoader{};

	private:
		std::shared_ptr<mpc::ui::Uis> uis;
		std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
		std::shared_ptr<controls::Controls> controls;

	private:
		std::shared_ptr<audiomidi::EventHandler> eventHandler;
		std::shared_ptr<sequencer::Sequencer> sequencer;
		std::shared_ptr<sampler::Sampler> sampler;
		std::shared_ptr<audiomidi::AudioMidiServices> audioMidiServices;
		std::vector<audiomidi::MpcMidiInput*> mpcMidiInputs;

	private:
		std::unique_ptr<mpc::disk::DiskController> diskController;

	private:
		std::shared_ptr<hardware::Hardware> hardware;
		int bank = 0;
		int pad = 0;
		int note = 60;
		int prevNote = 60;
		int prevPad = -1;

		// For returning to a specific screen after for example opening the Name screen
		std::string previousSamplerScreenName = "";

	public:
		void init(const int sampleRate, const int inputCount, const int outputCount);
		void setBank(int i);
		int getBank();
		void setPadAndNote(int pad, int note);
		int getNote();
		int getPad();
		int getPrevNote();
		int getPrevPad();
		std::string getPreviousSamplerScreenName();
		void setPreviousSamplerScreenName(std::string s);

	public:
		std::weak_ptr<ui::Uis> getUis();
		std::weak_ptr<lcdgui::LayeredScreen> getLayeredScreen();
		std::weak_ptr<controls::Controls> getControls();
		controls::BaseControls* getActiveControls();
		controls::GlobalReleaseControls* getReleaseControls();
		std::weak_ptr<hardware::Hardware> getHardware();

	public:
		std::weak_ptr<sequencer::Sequencer> getSequencer();
		std::weak_ptr<sampler::Sampler> getSampler();
		ctoot::mpc::MpcSoundPlayerChannel* getDrum(int i);
		ctoot::mpc::MpcBasicSoundPlayerChannel* getBasicPlayer();
		std::weak_ptr<audiomidi::AudioMidiServices> getAudioMidiServices();
		std::vector<ctoot::mpc::MpcSoundPlayerChannel*> getDrums();
		std::weak_ptr<audiomidi::EventHandler> getEventHandler();
		ctoot::mpc::MpcMultiMidiSynth* getMms();
		std::weak_ptr<mpc::audiomidi::MpcMidiPorts> getMidiPorts();
		mpc::audiomidi::MpcMidiInput* getMpcMidiInput(int i);


	public:
		void loadSound(bool replace);
		void loadProgram();
		void importLoadedProgram();

	private:
		void runLoadSoundThread(int size);
		static void static_loadSound(int size);

	public:
		std::weak_ptr<mpc::disk::AbstractDisk> getDisk();
		std::weak_ptr<mpc::disk::Stores> getStores();

	private:
		Mpc();
		~Mpc();

	public:
		static std::vector<char> akaiAsciiChar;
		static std::vector<std::string> akaiAscii;

	};
}
