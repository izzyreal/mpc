#pragma once

#include <string>
#include <vector>
#include <memory>

namespace mpc { class Mpc; }

namespace mpc::sequencer { class OldNoteEvent; class NoteOnEvent;}

namespace mpc::sampler { class Program; }

namespace mpc
{
	class Util
	{

	public:
		static std::vector<int> getPadAndVelo(const int x, const int y);
		static std::string getFileName(const std::string& s);
		static std::vector<std::string> splitName(const std::string& s);
		static std::string distributeTimeSig(const std::string& s);
		static std::string replaceDotWithSmallSpaceDot(const std::string& s);
		static void drawLine(std::vector<std::vector<bool>>& pixels, const std::vector<std::vector<int>>& line, const bool color);
		static void drawLine(std::vector<std::vector<bool>>& pixels, const std::vector<std::vector<int>>& line, const bool color, const std::vector<int>& offsetxy);
		static void drawLines(std::vector<std::vector<bool>>& pixels, const std::vector<std::vector<std::vector<int>>>& lines, const std::vector<bool>& colors);
		static void drawLines(std::vector<std::vector<bool>>& pixels, const std::vector<std::vector<std::vector<int>>>& lines, const std::vector<bool>& colors, const std::vector<int>& offsetxy);
		static std::vector<std::string>& noteNames();
		static std::string tempoString(const double tempo);
		static int getTextWidthInPixels(const std::string& text);
		static void initSequence(mpc::Mpc& mpc);
		static void initSequence(int sequenceIndex, mpc::Mpc& mpc);
		static void set16LevelsValues(mpc::Mpc&, const std::shared_ptr<mpc::sequencer::OldNoteEvent>&, const int padIndex);
		static void set16LevelsValues(mpc::Mpc&, const std::shared_ptr<mpc::sequencer::NoteOnEvent>&, const int padIndex);
        static void setSliderNoteVariationParameters(mpc::Mpc&, const std::weak_ptr<mpc::sequencer::OldNoteEvent>&, const std::weak_ptr<mpc::sampler::Program>&);
		static void setSliderNoteVariationParameters(mpc::Mpc&, const std::weak_ptr<mpc::sequencer::NoteOnEvent>&, const std::weak_ptr<mpc::sampler::Program>&);

	};
}
