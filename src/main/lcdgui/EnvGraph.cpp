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

void EnvGraph::setCoordinates(int attack, int decay)
{
	vector<int> line1{ 75, 43, 75 + (int)(attack * 0.17), 24 };
	vector<int> line2{ 119 - (int)(decay * 0.17), 24, 119, 43 };
	vector<int> line3{ 75 + (int)(attack * 0.17), 24, 119 - (int)(decay * 0.17), 24 };
	vector<vector<int>> lines = { line1, line2, line3 };
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
	int xoff = (csn.compare("programparams") == 0) ? 76 : 92;
	int yoff = (csn.compare("programparams") == 0) ? 16 : 22;
	int width = 44;
	int height = 28;
	for (int x = 0; x < width - 1; x++) {
		for (int y = 0; y < height - 1; y++) {
			int x1 = xoff + x;
			int y1 = yoff + y;
			if (csn.compare("programparams") == 0 && y1 == 24 && x1 >= 93 && x1 <= 101) continue;
			(*pixels)[x1][y1] = false;
		}
	}
	
	xoff = (csn.compare("programparams") == 0) ? 0 : 16;
	yoff = (csn.compare("programparams") == 0) ? 0 : 6;

	for (auto& c : coordinates) {
		auto linePixels = moduru::gui::Bressenham::Line(c[0] + xoff, c[1] + yoff, c[2] + xoff, c[3] + yoff);
		mpc::Util::drawLine(*pixels, linePixels, true);
	}
	dirty = false;
}
