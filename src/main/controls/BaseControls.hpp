#pragma once

#include "controls/PadPushContext.h"
#include "controls/GenerateNoteOnContext.h"
#include "controls/PadPressScreenUpdateContext.h"

#include <lcdgui/LayeredScreen.hpp>

#include <sequencer/Sequencer.hpp>

#include <sampler/Sampler.hpp>
#include <sampler/Pad.hpp>
#include <sampler/NoteParameters.hpp>

#include <controls/Controls.hpp>

#include <string>
#include <memory>
#include <vector>

namespace mpc { class Mpc; }
namespace mpc::controls { class GlobalReleaseControls; }

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
        const static std::vector<std::string> soundScreens;

        bool splittable = false;
		void splitLeft();
		void splitRight();

		std::vector<std::string> typableParams;

		std::shared_ptr<mpc::lcdgui::LayeredScreen> ls;
		std::shared_ptr<mpc::lcdgui::Field> activeField;

		void init();
		int getSoundIncrement(const int notch);
		virtual void left();
		virtual void right();
		virtual void up();
		virtual void down();
		virtual void function(const int i);
		virtual void numpad(const int i);
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
		virtual void bank(const int i);
		virtual void fullLevel();
		virtual void sixteenLevels();
		virtual void after();
		virtual void shift();
		virtual void undoSeq();
		virtual void erase();

		virtual bool isTypable();

		static void padPressScreenUpdate(PadPressScreenUpdateContext&, const int note, const std::optional<int> padIndexWithBank = std::nullopt);
		static void pad(PadPushContext&, const int padIndexWithBank, int velo);

		std::string getCurrentScreenName() { return currentScreenName; }

		const static std::vector<std::string> allowCentralNoteAndPadUpdateScreens;

	protected:
		std::string param;
		mpc::Mpc& mpc;
		std::weak_ptr<mpc::sequencer::Sequencer> sequencer;

	private:
		std::shared_ptr<mpc::sampler::Sampler> sampler;
		std::string currentScreenName;
		const static std::vector<std::string> screensThatAllowPlayAndRecord;
		const static std::vector<std::string> screensThatOnlyAllowPlay;

		static void generateNoteOn(const GenerateNoteOnContext& ctx, const int note, const int velo, const int padIndexWithBank);

        void handlePadHitInTrimLoopZoneParamsScreens();

		friend class mpc::lcdgui::ScreenComponent;
		friend class GlobalReleaseControls;
	};
}
