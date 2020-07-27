#include "EnvGraph.hpp"

#include <Mpc.hpp>
#include <lcdgui/LayeredScreen.hpp>

#include <gui/Bressenham.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui;
using namespace std;

EnvGraph::EnvGraph()
	: Component("env-graph")
{
	setSize(190, 40);
	setLocation(10, 10);
}

void EnvGraph::setCoordinates(int attack, int decay, bool decayModeStart)
{
	vector<int> line1{ 75, 43, 75 + (int)(attack * 0.2), 24 };

	vector<int> line2;
	vector<int> line3;

	vector<vector<int>> lines;

	if (decayModeStart)
	{
		auto x1 = 75 + (int)(attack * 0.2);
		line2 = { x1, 24, x1 + (int)(decay * 0.2), 43 };
		lines = { line1, line2 };
	}
	else
	{
		line2 = { 125 - (int)(decay * 0.2), 24, 125, 43 };
		line3 = { 75 + (int)(attack * 0.2), 24, 125 - (int)(decay * 0.2), 24 };
		lines = { line1, line2, line3 };
	}

	setCoordinates(lines);
}

void EnvGraph::setCoordinates(vector<vector<int>> ia)
{
	coordinates = ia;
	SetDirty();
}

void EnvGraph::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	auto csn = Mpc::instance().getLayeredScreen().lock()->getCurrentScreenName();
	int xoff = (csn.compare("program-params") == 0) ? 76 : 92;
	int yoff = (csn.compare("program-params") == 0) ? 16 : 22;

	int width = 49;
	int height = 27;
	
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int x1 = xoff + x;
			int y1 = yoff + y;
			
			(*pixels)[x1][y1] = false;
		}
	}
	
	xoff = (csn.compare("program-params") == 0) ? 0 : 16;
	yoff = (csn.compare("program-params") == 0) ? 0 : 6;

	for (auto& c : coordinates)
	{
		auto linePixels = moduru::gui::Bressenham::Line(c[0] + xoff, c[1] + yoff, c[2] + xoff, c[3] + yoff);
		mpc::Util::drawLine(*pixels, linePixels, true);
	}
	dirty = false;
}
