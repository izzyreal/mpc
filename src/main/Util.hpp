#pragma once
#include <string>
#include <vector>

#include <lang/StrUtil.hpp>

using namespace std;

namespace mpc {

	class Util {

	public:

		static vector<int> getPadAndVelo(const int x, const int y);
		static string getFileName(const string& s);
		static vector<string> splitName(const string& s);
		static string distributeTimeSig(const string& s);
		static string replaceDotWithSmallSpaceDot(const string& s);
		static void drawLine(vector<vector<bool>>& pixels, const vector<vector<int>>& line, const bool color);
		static void drawLine(vector<vector<bool>>& pixels, const vector<vector<int>>& line, const bool color, const vector<int>& offsetxy);
		static void drawLines(vector<vector<bool>>& pixels, const vector<vector<vector<int>>>& lines, const vector<bool>& colors);
		static void drawLines(vector<vector<bool>>& pixels, const vector<vector<vector<int>>>& lines, const vector<bool>& colors, const vector<int>& offsetxy);
		//static void drawScaled(IGraphics* g, vector<vector<int>> line, unsigned int scale, IColor* color, vector<int> offsetxy);
		//static void drawScaled(IGraphics* g, vector<vector<vector<int>>> lines, unsigned int scale, IColor* color, vector<int> offsetxy);
		//static void drawScaled(IGraphics* g, vector<vector<vector<int>>> lines, unsigned int scale, vector<IColor*> colors, vector<int> offsetxy);
		
	};
}
