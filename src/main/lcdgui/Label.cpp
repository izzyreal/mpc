#include "Label.hpp"

using namespace mpc::lcdgui;
using namespace std;

#include <Logger.hpp>

Label::Label(const string& name, string text, int x, int y, int width)
	: TextComp(name)
{
	inverted = false;
	Hide(false);
	this->text = text;
	this->name = name;
	this->width = width - 1;
	setText(text);
	setLocation(x, y);
	setSize(width, FONT_HEIGHT);
}

Label::~Label() {
}
