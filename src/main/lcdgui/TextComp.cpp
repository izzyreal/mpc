#include "TextComp.hpp"

#include <Mpc.hpp>
#include <Paths.hpp>

#include <lcdgui/Label.hpp>

#include <lang/StrUtil.hpp>
#include <lang/utf8_decode.h>

#include <cmath>

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

	if (text.length() == 0)
	{
		dirty = false;
		return;
	}

	if (dynamic_cast<Label*>(this) != nullptr)
	{
		auto rect = getRect();
		for (int x1 = rect.L + 1; x1 < rect.R; x1++)
		{
			for (int y1 = rect.T; y1 < rect.B - 1; y1++)
			{
				(*pixels)[x1][y1] = inverted ? true : false;
			}
		}
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
				bool on = (*atlas)[atlasy + y1][atlasx + x1];
			
				if (on)
				{
					int xpos = textx + x1 + current_char.xoffset;
					int ypos = texty + y1 + current_char.yoffset;

					if (h <= 7)
					{
						ypos--;
					}

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

int TextComp::getX()
{
	return x;
}

int TextComp::getY()
{
	return y;
}

int TextComp::getW()
{
	return w;
}

int TextComp::getH()
{
	return h;
}

void TextComp::setInverted(bool b)
{
	if (inverted != b)
	{
		inverted = b;
		SetDirty();
	}
}

string TextComp::getName()
{
	return name;
}

string TextComp::getText()
{
	return text;
}

unsigned int TextComp::GetTextEntryLength()
{
	return text.length();
}

void TextComp::setText(const string& s)
{
	text = s;
	SetDirty();
}

void TextComp::setTextPadded(string s, string padding)
{
	string padded = StrUtil::padLeft(s, padding, ceil(float(w - 2) / float(6)));
	setText(padded);
}

void TextComp::setTextPadded(int i, string padding)
{
	setTextPadded(to_string(i), padding);
}

void TextComp::static_blink(void* args)
{
	static_cast<TextComp*>(args)->runBlinkThread();
}

void TextComp::runBlinkThread()
{
	while (blinking)
	{
		int counter = 0;
		
		while (blinking && counter++ != BLINK_INTERVAL)
		{
			this_thread::sleep_for(chrono::milliseconds(1));
		}

		Hide(!IsHidden());
	}
	Hide(false);
}

void TextComp::setBlinking(bool b)
{
	if (blinking == b)
	{
		return;
	}

	blinking = b;

	if (blinkThread.joinable())
	{
		blinkThread.join();
	}

	if (blinking)
	{
		blinkThread = thread(&TextComp::static_blink, this);
	}
}

TextComp::~TextComp()
{
	if (blinking)
	{
		blinking = false;
		blinkThread.join();
	}
}
