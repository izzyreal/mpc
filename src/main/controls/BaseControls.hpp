#pragma once

#include "controls/PadPushContext.h"
#include "controls/GenerateNoteOnContext.h"
#include "controls/PadPressScreenUpdateContext.h"

#include <lcdgui/LayeredScreen.hpp>
#include "lcdgui/ScreenGroups.h"

#include <sequencer/Sequencer.hpp>

#include <sampler/Sampler.hpp>
#include <sampler/Pad.hpp>
#include <sampler/NoteParameters.hpp>

#include <controls/Controls.hpp>

#include <string>
#include <vector>
#include <unordered_map>

namespace mpc { class Mpc; }

namespace mpc::lcdgui
{
	class ScreenComponent;
}

namespace mpc::controls {
		
	class BaseControls {
	public:
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

		static int getSoundIncrement(const int64_t frameCount, const int notch);
        static void splitLeft(mpc::Mpc&);
        static void splitRight(mpc::Mpc&);
		static void left(mpc::Mpc&);
		static void right(mpc::Mpc&);
		static void up(mpc::Mpc&);
		static void down(mpc::Mpc&);
		static void function(mpc::Mpc&, const int i);
		static void numpad(mpc::Mpc&, const int i);
		static void pressEnter(mpc::Mpc&);
		static void rec(mpc::Mpc&);
		static void overDub(mpc::Mpc&);
		static void stop(mpc::Mpc&);
		static void play(mpc::Mpc&);
		static void playStart(mpc::Mpc&);
		static void mainScreen(mpc::Mpc&);
		static void tap(mpc::Mpc&);
		static void goTo(mpc::Mpc&);
		static void nextSeq(mpc::Mpc&);
		static void trackMute(mpc::Mpc&);
		static void bank(mpc::Mpc&, const int i);
		static void fullLevel(mpc::Mpc&);
		static void sixteenLevels(mpc::Mpc&);
		static void after(mpc::Mpc&);
		static void shift(mpc::Mpc&);
		static void undoSeq(mpc::Mpc&);
		static void erase(mpc::Mpc&);

		static void padPressScreenUpdate(PadPressScreenUpdateContext&, const int note, const std::optional<int> padIndexWithBank = std::nullopt);
		static void pad(PadPushContext&, const int padIndexWithBank, int velo);

	private:
		static void generateNoteOn(const GenerateNoteOnContext& ctx, const int note, const int velo, const int padIndexWithBank);
	};
}
