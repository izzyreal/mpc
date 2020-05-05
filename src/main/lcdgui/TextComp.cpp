#include "TextComp.hpp"

#include <Mpc.hpp>
#include <StartUp.hpp>

#include <lang/StrUtil.hpp>
#include <lang/utf8_decode.h>


using namespace mpc::lcdgui;

using namespace moduru::lang;
using namespace std;

TextComp::TextComp(std::vector<std::vector<bool>>* atlas, moduru::gui::bmfont* font)
{
	this->atlas = atlas;
	this->font = font;
}

void TextComp::Hide(bool b) {
	const int margin = noLeftMargin ? 0 : 1;
	if (b) {
		MRECT clearRect(x, y, x + (TEXT_WIDTH * columns) + margin, y + TEXT_HEIGHT + 1);
		clearRects.push_back(clearRect);
		dirtyRect = dirtyRect.Union(&clearRect);
	}
	Component::Hide(b);
}

void TextComp::Draw(std::vector<std::vector<bool>>* pixels) {

	int textx = x;
	int texty = y;

	int atlasx, atlasy;

	char* tempText = new char[text.length() + 1];
	std::strcpy(tempText, text.c_str());
	utf8_decode_init(tempText, text.length());

	int next = utf8_decode_next();
	int charCounter = 0;
	if (!IsHidden()) {
		while (next != UTF8_END && next >= 0) {
			moduru::gui::bmfont_char current_char;
			current_char = font->chars[next];
			atlasx = current_char.x;
			atlasy = current_char.y;
			for (int x1 = 0; x1 < current_char.width; x1++) {
				for (int y1 = 0; y1 < current_char.height; y1++) {
					bool on = (*atlas)[atlasx + x1][atlasy + y1 + 1];
					if (on) {
						int xpos = textx + x1 + current_char.xoffset;
						int ypos = texty + y1 + current_char.yoffset;
						if (xpos < 0 || xpos > 247 || ypos < 0 || ypos > 247) continue;
						(*pixels)[xpos][ypos] = inverted ? false : true;
					}
				}
			}
			textx += current_char.xadvance;
			next = utf8_decode_next();
		}
		delete tempText;
	}

	dirtyRect = dirtyRect.Union(&rect);

	dirty = false;
}

void TextComp::setSize(int w, int h, bool clear) {
	const int margin = noLeftMargin ? 0 : 1;
	
	if (clear) {
		MRECT clearRect(x, y, x + this->w + margin, y + TEXT_HEIGHT + 1);
		clearRects.push_back(clearRect);
		dirtyRect = dirtyRect.Union(&clearRect);
	}

	this->w = w;
	this->h = h;
	initRECT();
	SetDirty();
}

void TextComp::setLocation(int x, int y, bool clear) {
	
	if (clear) {
		const int margin = noLeftMargin ? 0 : 1;
		MRECT clearRect(this->x, this->y, this->x + w + margin, this->y + TEXT_HEIGHT + 1);
		clearRects.push_back(clearRect);
		dirtyRect = dirtyRect.Union(&clearRect);
	}

	this->x = x;
	this->y = y;
	initRECT();
	SetDirty();
}

void TextComp::initRECT() {
	rect = MRECT(x, y, x + w, y + h);
	dirtyRect = dirtyRect.Union(&rect);
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
	opaque = b;
	SetDirty();
}

void TextComp::setInverted(bool b) {
	inverted = b;
	SetDirty();
}

void TextComp::setNoLeftMargin(bool b) {
	noLeftMargin = true;
	SetDirty();
}

string TextComp::getName() {
	return name;
}

string TextComp::getText() {
	return text;
}

int TextComp::getColumns() {
	return columns;
}

void TextComp::setColumns(int i) {
	columns = i;
}

unsigned int TextComp::GetTextEntryLength() {
	return text.length();
}

void TextComp::setText(const string& s)
{
	text = s;
	//bool wasScrolling = scrolling;
	//if (scrolling) setScrolling(false);
	//auto typeName = typeid(*this).name();
	//auto typeStr = string(typeName);
	//text = s;
	//int len = 0;
	//auto s1 = text.c_str();
	//while (*s1) len += (*s1++ & 0xc0) != 0x80;

	//if (len > getColumns()) s = StrUtil::subStr(s, 0, getColumns());
	////if (textControl) {
	////	textControl->SetTextFromPlug(&s[0]);
	////}
	//if (wasScrolling) setScrolling(true);
	SetDirty();
}

void TextComp::setTextPadded(string s, string padding) {
	string padded = StrUtil::padLeft(s, padding, columns);
	setText(padded);
}

void TextComp::setTextPadded(int i, string padding) {
	setTextPadded(to_string(i), padding);
}

void TextComp::setScrolling(bool scrolling) {
	this->scrolling = scrolling;
	if (scrolling) {
		if (text.length() <= columns) {
			scrolling = false;
			return;
		}
		scrollThread = thread(&TextComp::static_scroll, this);
	}
	else {
		if (scrollThread.joinable()) scrollThread.join();
	}
}

void TextComp::scroll() {
	auto origText = this->text;
	int len = 0;
	auto s = origText.c_str();
	while (*s) len += (*s++ & 0xc0) != 0x80;
	int clmns = getColumns();
	bool left = false;
	int offset = 0;
	while (scrolling) {
		if (offset + clmns == len || offset == 0) {
			left = !left;
			const unsigned int repeats = 100;
			unsigned int counter = 0;
			while (scrolling && counter++ < repeats)
				this_thread::sleep_for(chrono::milliseconds(5));
		}
		offset = left ? offset + 1 : offset - 1;
		auto tmp = StrUtil::subStr(origText, offset, offset + clmns);
		//textControl->SetTextFromPlug(&tmp[0]);
		//SetDirty(true);
		const unsigned int repeats = 100;
		unsigned int counter = 0;
		while (scrolling && counter++ < repeats)
			this_thread::sleep_for(chrono::milliseconds(3));
	}
}

void TextComp::static_scroll(void * args) {
	static_cast<TextComp*>(args)->scroll();
}

TextComp::~TextComp() {
	if (scrollThread.joinable()) scrollThread.join();
}
