#include <lcdgui/EnvGraph.hpp>

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

void EnvGraph::setCoordinates(vector<vector<int>> ia)
{
	coordinates = ia;
	SetDirty();
}

void EnvGraph::Draw(std::vector<std::vector<bool>>* pixels)
{
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

EnvGraph::~EnvGraph() {
}
