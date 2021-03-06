#include "Label.hpp"

using namespace mpc::lcdgui;
using namespace std;

Label::Label(mpc::Mpc& mpc, const string& name, string text, int x, int y, int width)
	: TextComp(mpc, name)
{
	this->text = text;
	this->name = name;
	setText(text);
	setSize(width + 2, 9);
	setLocation(x - 1, y);
	preDrawClearRect.Clear();
	dirty = false;
}
