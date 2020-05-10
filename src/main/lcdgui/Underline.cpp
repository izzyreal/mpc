#include <lcdgui/Underline.hpp>

#include <Logger.hpp>

using namespace mpc::lcdgui;
using namespace std;

Underline::Underline()
	: Component("underline")
{
	states = vector<bool>(16);
	setSize(248, 50);
	setLocation(0, 0);
}

void Underline::Draw(vector<vector<bool>>* pixels)
{
	if (hidden || !IsDirty())
	{
		return;
	}

	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 6; j++) {
			(*pixels)[j + (i * 6) + 106][27] = states[i];
		}
	}
	dirty = false;
}

void Underline::setState(int i, bool b)
{
    states[i] = b;
	SetDirty();
}

Underline::~Underline() {
}
