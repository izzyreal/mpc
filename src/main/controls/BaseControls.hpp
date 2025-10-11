#pragma once

#include "controls/PadPushContext.h"
#include "controls/GenerateNoteOnContext.h"
#include "controls/PadPressScreenUpdateContext.h"

#include <lcdgui/LayeredScreen.hpp>
#include "lcdgui/Layer.hpp"
#include "lcdgui/ScreenGroups.h"

#include <sequencer/Sequencer.hpp>

#include <sampler/Sampler.hpp>
#include <sampler/Pad.hpp>
#include <sampler/NoteParameters.hpp>

#include <controls/Controls.hpp>

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

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

        static std::string getCurrentScreenName(mpc::Mpc &mpc) { return mpc.getLayeredScreen()->getCurrentScreenName(); }
        static std::string getPreviousScreenName(mpc::Mpc &mpc) { return mpc.getLayeredScreen()->getPreviousScreenName(); }
        static std::string getLastFocusedField(mpc::Mpc &mpc, const std::string screenName) { return mpc.getLayeredScreen()->getLastFocus(screenName); }

        static bool isFieldSplittable(const std::string screenName, const std::string fieldName)
        {
            return lcdgui::screengroups::isSamplerScreen(screenName) &&
                (fieldName == "st" ||
                 fieldName == "end" ||
                 fieldName == "to" ||
                 fieldName == "endlengthvalue" ||
                 fieldName == "start");
        }

        static std::string getFocusedFieldName(mpc::Mpc &mpc) { return mpc.getLayeredScreen()->getFocus(); }

        static std::shared_ptr<lcdgui::Field> getFocusedField(mpc::Mpc &mpc)
        {
            return mpc.getLayeredScreen()->getFocusedLayer()->findField(getFocusedFieldName(mpc));
        }

        static bool isTypableField(const std::string screenName, const std::string fieldName)
        {
            for (auto &f : getTypableFields(screenName))
            {
                if (f == fieldName) return true;
            }

            return false;
        }

		static std::vector<std::string> getTypableFields(const std::string screenName)
        {
            static const std::unordered_map<std::string, std::vector<std::string>> typableFields {
                { "sequencer",      { "tempo", "now0", "now1", "now2", "velo" } },
                { "trim",           { "st", "end" } },
                { "loop-end-fine",  { "end", "lngth" } },
                { "start-fine",     { "start" } },
                { "zone-end-fine",  { "end" } },
                { "end-fine",       { "end" } },
                { "zone-start-fine",{ "start" } },
                { "loop-to-fine",   { "to", "lngth" } },
                { "zone",           { "st", "end" } },
                { "loop",           { "to", "endlengthvalue" } },
            };

            if (typableFields.count(screenName) == 0) return {};

            return typableFields.at(screenName);
        }

		void splitLeft();
		void splitRight();

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

		static void padPressScreenUpdate(PadPressScreenUpdateContext&, const int note, const std::optional<int> padIndexWithBank = std::nullopt);
		static void pad(PadPushContext&, const int padIndexWithBank, int velo);

	protected:
		mpc::Mpc& mpc;
		std::weak_ptr<mpc::sequencer::Sequencer> sequencer;

	private:
		std::shared_ptr<mpc::sampler::Sampler> sampler;
		static void generateNoteOn(const GenerateNoteOnContext& ctx, const int note, const int velo, const int padIndexWithBank);

        void handlePadHitInTrimLoopZoneParamsScreens();

		friend class mpc::lcdgui::ScreenComponent;
		friend class GlobalReleaseControls;
	};
}
