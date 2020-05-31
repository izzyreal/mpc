#include "Label.hpp"

using namespace mpc::lcdgui;
using namespace std;

Label::Label(const string& name, string text, int x, int y, int width)
	: TextComp(name)
{
	this->text = text;
	this->name = name;
	setText(text);
	setSize(width + 2, 9);
	setLocation(x - 1, y - 1);
}
