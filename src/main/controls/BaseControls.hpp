#pragma once

#include <lcdgui/LayeredScreen.hpp>

#include <sequencer/Sequencer.hpp>

#include <sampler/Sampler.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/NoteParameters.hpp>

#include <controls/Controls.hpp>

#include <string>
#include <memory>
#include <vector>

namespace ctoot::mpc { class MpcSoundPlayerChannel; }
namespace mpc { class Mpc; }
namespace mpc::controls { class GlobalReleaseControls; }

namespace mpc::sequencer
{
	class Track;
	class NoteEvent;
}

namespace mpc::sampler { class Program; }

namespace mpc::lcdgui
{
	class Field;
	class LayeredScreen;
	class ScreenComponent;
}

namespace mpc::controls {
		
	class BaseControls {
	public:
		BaseControls(mpc::Mpc&);

		bool splittable = false;
		void splitLeft();
		void splitRight();

		std::vector<std::string> typableParams;

		std::weak_ptr<mpc::sampler::Program> program;
		ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel = nullptr;

		std::weak_ptr<mpc::lcdgui::LayeredScreen> ls;
		std::weak_ptr<mpc::lcdgui::Field> activeField;

		virtual void init();
		int getSoundIncrement(int notch);
		virtual void left();
		virtual void right();
		virtual void up();
		virtual void down();
		virtual void function(int i);
		virtual void openWindow();
		virtual void turnWheel(int i);
		virtual void numpad(int i);
		virtual void pressEnter();
		virtual void rec();
		virtual void overDub();
		virtual void stop();
		virtual void play();
		virtual void playStart();
		virtual void mainScreen();
		virtual void tap();
		virtual void prevStepEvent();
		virtual void nextStepEvent();
		virtual void goTo();
		virtual void prevBarStart();
		virtual void nextBarEnd();
		virtual void nextSeq();
		virtual void trackMute();
		virtual void bank(int i);
		virtual void fullLevel();
		virtual void sixteenLevels();
		virtual void after();
		virtual void shift();
		virtual void undoSeq();
		virtual void erase();
		virtual void setSlider(int) {}

		virtual bool isTypable();

		virtual void pad(int i, int velo, bool repeat, int tick);

	protected:
		std::string param = "";
		mpc::Mpc& mpc;
		std::weak_ptr<mpc::sequencer::Sequencer> sequencer;

	private:
		std::weak_ptr<mpc::sampler::Sampler> sampler;
		std::weak_ptr<mpc::sequencer::Track> track;
		std::string currentScreenName = "";
		const static std::vector<std::string> allowTransportScreens;
		const static std::vector<std::string> allowPlayScreens;
        const static std::vector<std::string> samplerScreens;

		bool allowTransport();
		bool allowPlay();
        bool isSamplerScreen();
		void generateNoteOn(int nn, int padVelo, int tick);

		friend class mpc::lcdgui::ScreenComponent;
		friend class mpc::controls::GlobalReleaseControls;
	};
}
