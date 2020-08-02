#include "TextComp.hpp"

#include <Mpc.hpp>
#include <Paths.hpp>

#include <lcdgui/Label.hpp>
#include <lcdgui/Field.hpp>

#include <Util.hpp>

#include <lang/StrUtil.hpp>
#include <lang/utf8_decode.h>

#include <cmath>

using namespace mpc::lcdgui;
using namespace moduru::lang;
using namespace std;

TextComp::TextComp(mpc::Mpc& mpc, const std::string& name)
	: Component(name), mpc(mpc)
{
}

void TextComp::enableTwoDots()
{
	if (twoDots)
		return;

	twoDots = true;
	SetDirty();
}

void TextComp::setSize(int w, int h)
{
	if (alignmentEndX == -1)
	{
		alignmentEndX = w;
	}
	Component::setSize(w, h);
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

	auto& font = mpc.getLayeredScreen().lock()->font;
	auto& atlas = mpc.getLayeredScreen().lock()->atlas;

	int textx = x + 1;
	int texty = y;

	int alignmentOffset = 0;

	string textToRender = text;

	if (alignment == Alignment::Centered && !textuallyAligned)
	{
		auto charsToAlignCount = min(int(ceil(alignmentEndX / float(FONT_WIDTH))), (int)text.length());
		auto charsToAlign = StrUtil::replaceAll(text.substr(0, charsToAlignCount), ' ', "");
		textToRender = charsToAlign + text.substr(charsToAlignCount);
		auto charsWidthInPixels = mpc::Util::getTextWidthInPixels(charsToAlign);
		alignmentOffset = (alignmentEndX - charsWidthInPixels) * 0.5;
	}

	int atlasx, atlasy;

	char* tempText = new char[textToRender.length() + 1];
	std::strcpy(tempText, textToRender.c_str());
	utf8_decode_init(tempText, textToRender.length());

	int next = utf8_decode_next();
	int charCounter = 0;
	
	auto field = dynamic_cast<Field*>(this);

	while (next != UTF8_END && next >= 0)
	{
		moduru::gui::bmfont_char current_char;
		current_char = font.chars[next];
		atlasx = current_char.x;
		atlasy = current_char.y;
		
		for (int x1 = 0; x1 < current_char.width; x1++)
		{
			for (int y1 = 0; y1 < current_char.height; y1++)
			{
				bool on = atlas[atlasy + y1][atlasx + x1];
			
				if (on)
				{
					int xpos = textx + x1 + current_char.xoffset;
					int ypos = texty + y1 + current_char.yoffset;

					if (alignment == Alignment::Centered && !textuallyAligned) 
					{
						if (xpos - x < alignmentEndX - 4)
						{
							xpos += alignmentOffset;
						}
						else {
							xpos += (2 * alignmentOffset);
						}
					}

					if (w == 47 && name.find("note") != string::npos)
					{
						// Super hacky way to cram as much text in the amount of pixels that
						// the original leet coders of the Akai MPC2000XL OS did. Respect.
						xpos -= 1;
					}

					if (h <= 7)
					{
						ypos--;
					}

					(*pixels)[xpos][ypos] = inverted ? false : true;

					if (field != nullptr && field->isSplit())
						(*pixels)[xpos][ypos] = charCounter > field->getActiveSplit();

				}
			}
		}
		textx += current_char.xadvance;
		next = utf8_decode_next();
		charCounter++;
	}

	delete[] tempText;
	
	if (twoDots)
	{
		for (auto xPos : vector<int>{ 12, 30 })
		{
			if (w > xPos)
				(*pixels)[xPos + x][y + 8] = inverted ? false : true;
		}
	}

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

	if (alignment == Alignment::Centered && alignmentEndX != w)
	{
		auto charsToAlignCount = min(int(ceil(alignmentEndX / float(FONT_WIDTH))), (int)text.length());
		auto charsToAlign = StrUtil::replaceAll(text.substr(0, charsToAlignCount), ' ', "");
		
		if ((charsToAlignCount - charsToAlign.length()) % 2 == 0)
		{
			text = s.substr(charsToAlignCount);
			auto nudge = (charsToAlignCount - charsToAlign.length()) / 2;
			
			for (int i = 0; i < nudge; i++)
				text.insert(text.begin(), ' ');

			for (int i = nudge; i < nudge + charsToAlign.length(); i++)
				text.insert(text.begin() + i, charsToAlign[i - nudge]);

			for (int i = nudge + charsToAlign.length(); i < charsToAlignCount; i++)
				text.insert(text.begin() + i, ' ');

			textuallyAligned = true;
		}
		else
		{
			textuallyAligned = false;
		}
	}

	SetDirty();
}

void TextComp::setTextPadded(string s, string padding)
{
	auto columns = (int)floor(w / FONT_WIDTH);
	string padded = StrUtil::padLeft(s, padding, columns);
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

void TextComp::setAlignment(const Alignment alignment, int endX)
{
	this->alignment = alignment;
	alignmentEndX = endX;

	if (alignmentEndX == -1)
	{
		alignmentEndX = w;
	}

	SetDirty();
}

TextComp::~TextComp()
{
	if (blinking)
	{
		blinking = false;
		blinkThread.join();
	}
}
