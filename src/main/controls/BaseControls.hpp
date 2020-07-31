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

namespace ctoot::mpc
{
	class MpcSoundPlayerChannel;
}

namespace mpc
{
	class Mpc;
}

namespace mpc::sequencer
{
	class Track;
	class NoteEvent;
}

namespace mpc::sampler
{
	class Program;
}

namespace mpc::lcdgui
{
	class Field;
	class LayeredScreen;
}

namespace mpc::controls
{
	class BaseControls
	{

	protected:
		mpc::Mpc& mpc;
		std::weak_ptr<mpc::sequencer::Sequencer> sequencer;
		std::weak_ptr<mpc::sampler::Sampler> sampler;

	public:
		bool splittable = false;
		static std::vector<int> splitInc;
		void splitLeft();
		void splitRight();

	public:
		std::string param = "";
		std::string currentScreenName = "";

		std::weak_ptr<mpc::sequencer::Track> track;
		std::weak_ptr<mpc::sampler::Program> program;
		ctoot::mpc::MpcSoundPlayerChannel* mpcSoundPlayerChannel = nullptr;

		std::weak_ptr<mpc::lcdgui::LayeredScreen> ls;
		std::weak_ptr<mpc::lcdgui::Field> activeField;

		std::vector<std::string> typableParams;

	public:
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
		virtual void setSlider(int i) {};

		virtual bool isTypable();

		virtual void pad(int i, int velo, bool repeat, int tick);
		void generateNoteOn(int nn, int padVelo, int tick);

		void setSliderNoteVar(mpc::sequencer::NoteEvent* n, std::weak_ptr<mpc::sampler::Program> program);

	public:
		BaseControls(mpc::Mpc& mpc);

	};
}
