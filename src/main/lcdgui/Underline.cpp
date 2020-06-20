#include <lcdgui/Underline.hpp>

using namespace mpc::lcdgui;
using namespace std;

Underline::Underline()
	: Component("underline")
{
	states = vector<bool>(16);
	setSize(95, 1);
	setLocation(106, 27);
}

void Underline::Draw(vector<vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			(*pixels)[j + (i * 6) + x][y] = states[i];
		}
	}
	dirty = false;
}

void Underline::setState(int i, bool b)
{
    states[i] = b;
	SetDirty();
}
