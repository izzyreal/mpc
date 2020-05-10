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
	setSize(184, 17);
	setLocation(34, 18);
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
	lcdgui::Label l("popup", text, 43, 23, text.size() * 6);
	l.setOpaque(true);
	l.setInverted(true);
	l.Draw(pixels);
	dirty = false;
}

Popup::~Popup() {
}
