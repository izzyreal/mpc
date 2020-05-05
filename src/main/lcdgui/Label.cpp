#include "Label.hpp"

using namespace mpc::lcdgui;
using namespace std;

Label::Label(std::vector<std::vector<bool>>* atlas, moduru::gui::bmfont* font)
	: TextComp(atlas, font)
{
}

void Label::initialize(string name, string text, int x, int y, int columns) {
	rigorousClearing = false;
	noLeftMargin = false;
	opaque = true;
	inverted = false;
	Hide(false);
	this->text = text;
	this->name = name;
	this->columns = columns;
	setText(text);
	setLocation(x, y, false);
	setSize(columns * TEXT_WIDTH + 1, TEXT_HEIGHT + 1, false);
	clearRects.clear();
}

void Label::enableRigorousClearing() {
	rigorousClearing = true;
}

void Label::Draw(vector<vector<bool>>* pixels) {
	const int margin = noLeftMargin ? 0 : 1;
	if (rigorousClearing) {
		for (int j = 0; j < w; j++) {
			for (int k = 0; k < TEXT_HEIGHT + 2; k++) {
				int x1 = x + j - margin;
				int y1 = y + k;
				if (x1 < 0 || x1 > 247 || y1 < 0 || y1 > 59) continue;
				(*pixels)[x1][y1] = inverted ? true : false;
			}
		}
	}
	TextComp::Draw(pixels);
}

Label::~Label() {
}
