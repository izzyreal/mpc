#pragma once
#include <string>
#include <vector>

#include <lang/StrUtil.hpp>
//#include <IGraphics.h>

namespace mpc {

	class Util {

	public:

		static std::vector<int> getPadAndVelo(int x, int y);
		static std::string getFileName(std::string s);
		static std::string* splitName(std::string s);
		static std::string distributeTimeSig(std::string s);
		static std::string replaceDotWithSmallSpaceDot(std::string s);
		static void drawLine(std::vector<std::vector<bool> >* pixels, std::vector<std::vector<int> >* line, bool color);
		static void drawLine(std::vector<std::vector<bool> >* pixels, std::vector<std::vector<int> >* line, bool color, std::vector<int>* offsetxy);
		static void drawLines(std::vector<std::vector<bool> >* pixels, std::vector<std::vector<std::vector<int> > >* lines, std::vector<bool> colors);
		static void drawLines(std::vector<std::vector<bool> >* pixels, std::vector<std::vector<std::vector<int> > >* lines, std::vector<bool> colors, std::vector<int>* offsetxy);
		//static void drawScaled(IGraphics* g, std::vector<std::vector<int>> line, unsigned int scale, IColor* color, std::vector<int> offsetxy);
		//static void drawScaled(IGraphics* g, std::vector<std::vector<std::vector<int>>> lines, unsigned int scale, IColor* color, std::vector<int> offsetxy);
		//static void drawScaled(IGraphics* g, std::vector<std::vector<std::vector<int>>> lines, unsigned int scale, std::vector<IColor*> colors, std::vector<int> offsetxy);
		
	};
}
