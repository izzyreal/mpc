#include <lcdgui/Underline.hpp>

#include <Logger.hpp>

using namespace mpc::lcdgui;
using namespace std;

Underline::Underline()
{
	states = vector<bool>(16);
	rect = MRECT(0, 0, 247, 50);
}

void Underline::Draw(vector<vector<bool>>* pixels)
{
	if (IsHidden()) return;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 6; j++) {
			pixels->at(j + (i * 6) + 106)[27] = states[i];
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
