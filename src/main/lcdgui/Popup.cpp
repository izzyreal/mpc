#include "Popup.hpp"

#include <lcdgui/Background.hpp>
#include <lcdgui/Label.hpp>

#include <cmath>

using namespace mpc::lcdgui;
using namespace std;

Popup::Popup()
	: Component("popup")
{
	bg = make_unique<Background>();
	bg->setName("popup");
	//int x = maingui::Constants::POPUP_RECT()->L + 20;
	//int y = maingui::Constants::POPUP_RECT()->T + 12;
	int textWidth = 200;
	int textHeight = 8;
	//textRect = IRECT(x, y, x + textWidth, y + textHeight);
	
	rect = MRECT(34, 18, 34 + 184, 18 + 17);
}

void Popup::setText(string text, int pos)
{
	this->text = text;
	this->pos = pos;
	SetDirty();
}

void Popup::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (IsHidden()) {
		return;
	}

	bg->Draw(pixels);
	lcdgui::Label l("popup", text, 43, 23, text.size());
	l.setOpaque(true);
	l.setInverted(true);
	l.Draw(pixels);
	dirty = false;
}

Popup::~Popup() {
}
