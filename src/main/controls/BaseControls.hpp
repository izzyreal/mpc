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

namespace mpc::engine { class Drum; }
namespace mpc { class Mpc; }
namespace mpc::controls { class GlobalReleaseControls; }

namespace mpc::sequencer
{
	class Track;
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

        bool currentScreenAllowsPlay();
        bool collectionContainsCurrentScreen(const std::vector<std::string>&);
        const static std::vector<std::string> samplerScreens;

        bool splittable = false;
		void splitLeft();
		void splitRight();

		std::vector<std::string> typableParams;

		std::shared_ptr<mpc::sampler::Program> program;
		mpc::engine::Drum* activeDrum;

		std::shared_ptr<mpc::lcdgui::LayeredScreen> ls;
		std::shared_ptr<mpc::lcdgui::Field> activeField;

		void init();
		int getSoundIncrement(int notch);
		virtual void left();
		virtual void right();
		virtual void up();
		virtual void down();
		virtual void function(int i);
		virtual void numpad(int i);
		virtual void pressEnter();
		virtual void rec();
		virtual void overDub();
		virtual void stop();
		virtual void play();
		virtual void playStart();
		virtual void mainScreen();
		virtual void tap();
		virtual void prevStepEvent() {}
		virtual void nextStepEvent() {}
		virtual void goTo();
		virtual void prevBarStart() {}
		virtual void nextBarEnd() {}
		virtual void nextSeq();
		virtual void trackMute();
		virtual void bank(int i);
		virtual void fullLevel();
		virtual void sixteenLevels();
		virtual void after();
		virtual void shift();
		virtual void undoSeq();
		virtual void erase();

		virtual bool isTypable();

		void various(int note, std::optional<int> padIndexWithBank = std::nullopt);
		void pad(int padIndexWithBank, int velo);

		std::string getCurrentScreenName() { return currentScreenName; }

	protected:
		std::string param;
		mpc::Mpc& mpc;
		std::shared_ptr<mpc::sequencer::Sequencer> sequencer;

	private:
		std::shared_ptr<mpc::sampler::Sampler> sampler;
		std::shared_ptr<mpc::sequencer::Track> track;
		std::string currentScreenName;
		const static std::vector<std::string> screensThatAllowPlayAndRecord;
		const static std::vector<std::string> screensThatOnlyAllowPlay;
		const static std::vector<std::string> allowCentralNoteAndPadUpdateScreens;
		void generateNoteOn(int note, int velo, int padIndexWithBank);

		friend class mpc::lcdgui::ScreenComponent;
		friend class GlobalReleaseControls;
	};
}
