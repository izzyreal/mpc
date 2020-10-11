#pragma once
#include <string>
#include <vector>

namespace mpc { class Mpc; }

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

	};
}
