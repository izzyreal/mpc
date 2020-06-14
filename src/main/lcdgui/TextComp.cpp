#include "TextComp.hpp"

#include <Mpc.hpp>
#include <Paths.hpp>


#include <lang/StrUtil.hpp>
#include <lang/utf8_decode.h>


using namespace mpc::lcdgui;

using namespace moduru::lang;
using namespace std;

TextComp::TextComp(const std::string& name)
	: Component(name)
{
}

void TextComp::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	if (mpc::Mpc::instance().getLayeredScreen().lock()->getCurrentScreenName().compare("step-editor") == 0)
	{
		MLOG("Drawing " + name + " with width " + to_string(w));
	}

	auto font = &LayeredScreen::font;
	auto atlas = &LayeredScreen::atlas;

	int textx = x + 1;
	int texty = y;

	int atlasx, atlasy;

	char* tempText = new char[text.length() + 1];
	std::strcpy(tempText, text.c_str());
	utf8_decode_init(tempText, text.length());

	int next = utf8_decode_next();
	int charCounter = 0;
	
	while (next != UTF8_END && next >= 0)
	{
		moduru::gui::bmfont_char current_char;
		current_char = font->chars[next];
		atlasx = current_char.x;
		atlasy = current_char.y;
		
		for (int x1 = 0; x1 < current_char.width; x1++)
		{
			for (int y1 = 0; y1 < current_char.height; y1++)
			{
				bool on = (*atlas)[atlasx + x1][atlasy + y1 + 1];
			
				if (on)
				{
					int xpos = textx + x1 + current_char.xoffset;
					int ypos = texty + y1 + current_char.yoffset;
					(*pixels)[xpos][ypos] = inverted ? false : true;
				}
			}
		}
		textx += current_char.xadvance;
		next = utf8_decode_next();
	}
	delete[] tempText;
	dirty = false;
}

int TextComp::getX() {
	return x;
}

int TextComp::getY() {
	return y;
}

int TextComp::getW() {
	return w;
}

int TextComp::getH() {
	return h;
}

void TextComp::setOpaque(bool b) {
	if (opaque != b)
	{
		opaque = b;
		SetDirty();
	}
}

void TextComp::setInverted(bool b) {
	if (inverted != b)
	{
		inverted = b;
		SetDirty();
	}
}

string TextComp::getName() {
	return name;
}

string TextComp::getText() {
	return text;
}

unsigned int TextComp::GetTextEntryLength() {
	return text.length();
}

void TextComp::setText(const string& s)
{
	if (text.compare(s) != 0)
	{
		text = s;
		SetDirty();
	}
}

void TextComp::setTextPadded(string s, string padding) {
	string padded = StrUtil::padLeft(s, padding, ceil(float(w - 2) / float(6)));
	setText(padded);
}

void TextComp::setTextPadded(int i, string padding) {
	setTextPadded(to_string(i), padding);
}
