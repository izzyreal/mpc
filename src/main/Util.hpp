#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "mpc_types.hpp"
#include "sampler/PgmSlider.hpp"

namespace mpc { class Mpc; }

namespace mpc::sequencer { class NoteOnEvent; }

namespace mpc::sampler { class Program; }

namespace mpc
{
	class Util
	{

	public:
        struct SixteenLevelsContext {
            const bool isSixteenLevelsEnabled;
            const int type; // From Assign16LevelsScreen::getType()
            const int originalKeyPad; // From Assign16LevelsScreen::getOriginalKeyPad()
            const int note; // From Assign16LevelsScreen::getNote()
            const int parameter; // From Assign16LevelsScreen::getParameter()
            const int padIndexWithoutBank;
        };
        struct SliderNoteVariationContext {
            const int sliderValue; // From Hardware::getSlider()->getValue()
            const int programNote; // From Program::getSlider()->getNote()
            const int sliderParameter; // From Program::getSlider()->getParameter()
            const int tuneLowRange; // From Program::getSlider()->getTuneLowRange()
            const int tuneHighRange; // From Program::getSlider()->getTuneHighRange()
            const int decayLowRange; // From Program::getSlider()->getDecayLowRange()
            const int decayHighRange; // From Program::getSlider()->getDecayHighRange()
            const int attackLowRange; // From Program::getSlider()->getAttackLowRange()
            const int attackHighRange; // From Program::getSlider()->getAttackHighRange()
            const int filterLowRange; // From Program::getSlider()->getFilterLowRange()
            const int filterHighRange; // From Program::getSlider()->getFilterHighRange()
            mpc::sampler::PgmSlider *slider; 
        };
        static std::string getFileName(const std::string& s);
		static std::vector<std::string> splitName(const std::string& s);
		static std::string distributeTimeSig(const std::string& s);
		static std::string replaceDotWithSmallSpaceDot(const std::string& s);
		static void drawLine(std::vector<std::vector<bool>>& pixels, const LcdLine& line, const bool color);
		static void drawLine(std::vector<std::vector<bool>>& pixels, const LcdLine & line, const bool color, const std::vector<int>& offsetxy);
        static void drawLines(std::vector<std::vector<bool>>& pixels, const LcdBitmap& lines, const std::vector<bool>& colors, const std::vector<int>& offsetxy);
		static std::vector<std::string>& noteNames();
		static std::string tempoString(const double tempo);
		static int getTextWidthInPixels(const std::string& text);
		static void initSequence(mpc::Mpc& mpc);
		static void initSequence(int sequenceIndex, mpc::Mpc& mpc);
		static void set16LevelsValues(const SixteenLevelsContext&, const std::shared_ptr<mpc::sequencer::NoteOnEvent>);
		static void setSliderNoteVariationParameters(const SliderNoteVariationContext&, const std::shared_ptr<mpc::sequencer::NoteOnEvent>);

        static std::vector<char> vecCopyOfRange(const std::vector<char>& src, int offset, int length);
        static bool vecEquals(const std::vector<char>& a, const std::vector<char>& b);

    };
}
