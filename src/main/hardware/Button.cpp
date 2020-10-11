#include "Button.hpp"

#include <Mpc.hpp>

#include <lcdgui/ScreenComponent.hpp>

#include <controls/BaseControls.hpp>
#include <controls/GlobalReleaseControls.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::hardware;
using namespace std;

Button::Button(mpc::Mpc& mpc, string label)
	: mpc(mpc)
{
	this->label = label;
}

string Button::getLabel() {
	return label;
}

void Button::push()
{
	auto ls = mpc.getLayeredScreen().lock();

	if (mpc.getDisk().lock()->isBusy() && ls->getCurrentScreenName().compare("cant-find-file") != 0)
		return;

	auto controls = ls->findScreenComponent().lock();
	
	if (!controls)
		return;
	
	if (label.compare("left") == 0) {
		controls->left();
	}
	else if (label.compare("right") == 0) {
		controls->right();
	}
	else if (label.compare("up") == 0) {
		controls->up();
	}
	else if (label.compare("down") == 0) {
		controls->down();
	}
	else if (label.compare("rec") == 0) {
		controls->rec();
	}
	else if (label.compare("overdub") == 0) {
		controls->overDub();
	}
	else if (label.compare("stop") == 0) {
		controls->stop();
	}
	else if (label.compare("play") == 0) {
		controls->play();
	}
	else if (label.compare("play-start") == 0) {
		controls->playStart();
	}
	else if (label.compare("main-screen") == 0) {
		controls->mainScreen();
	}
	else if (label.compare("open-window") == 0) {
		controls->openWindow();
	}
	else if (label.compare("prev-step-event") == 0) {
		controls->prevStepEvent();
	}
	else if (label.compare("next-step-event") == 0) {
		controls->nextStepEvent();
	}
	else if (label.compare("go-to") == 0) {
		controls->goTo();
	}
	else if (label.compare("prev-bar-start") == 0) {
		controls->prevBarStart();
	}
	else if (label.compare("next-bar-end") == 0) {
		controls->nextBarEnd();
	}
	else if (label.compare("tap") == 0) {
		controls->tap();
	}
	else if (label.compare("next-seq") == 0) {
		controls->nextSeq();
	}
	else if (label.compare("track-mute") == 0) {
		controls->trackMute();
	}
	else if (label.compare("full-level") == 0) {
		controls->fullLevel();
	}
	else if (label.compare("sixteen-levels") == 0) {
		controls->sixteenLevels();
	}
	else if (label.compare("f1") == 0) {
		controls->function(0);
	}
	else if (label.compare("f2") == 0) {
		controls->function(1);
	}
	else if (label.compare("f3") == 0) {
		controls->function(2);
	}
	else if (label.compare("f4") == 0) {
		controls->function(3);
	}
	else if (label.compare("f5") == 0) {
		controls->function(4);
	}
	else if (label.compare("f6") == 0) {
		controls->function(5);
	}
	else if (label.compare("shift") == 0) {
		controls->shift();
	}
	else if (label.compare("enter") == 0) {
		controls->pressEnter();
	}
	else if (label.compare("undo-seq") == 0) {
		controls->undoSeq();
	}
	else if (label.compare("erase") == 0) {
		controls->erase();
	}
	else if (label.compare("after") == 0) {
		controls->after();
	}
	else if (label.compare("bank-a") == 0) {
		controls->bank(0);
	}
	else if (label.compare("bank-b") == 0) {
		controls->bank(1);
	}
	else if (label.compare("bank-c") == 0) {
		controls->bank(2);
	}
	else if (label.compare("bank-d") == 0) {
		controls->bank(3);
	}
	else if (label.compare("0") == 0) {
		controls->numpad(0);
	}
	else if (label.compare("1") == 0) {
		controls->numpad(1);
	}
	else if (label.compare("2") == 0) {
		controls->numpad(2);
	}
	else if (label.compare("3") == 0) {
		controls->numpad(3);
	}
	else if (label.compare("4") == 0) {
		controls->numpad(4);
	}
	else if (label.compare("5") == 0) {
		controls->numpad(5);
	}
	else if (label.compare("6") == 0) {
		controls->numpad(6);
	}
	else if (label.compare("7") == 0) {
		controls->numpad(7);
	}
	else if (label.compare("8") == 0) {
		controls->numpad(8);
	}
	else if (label.compare("9") == 0) {
		controls->numpad(9);
	}
}

void Button::release() {
	if (mpc.getDisk().lock()->isBusy()) return;
	auto c = mpc.getReleaseControls();
	if (label.compare("shift") == 0) {
		c->shift();
	}
	else if (label.compare("erase") == 0) {
		c->erase();
	}
	else if (label.compare("f1") == 0) {
		c->function(0);
	}
	else if (label.compare("f3") == 0) {
		c->function(2);
	}
	else if (label.compare("f4") == 0) {
		c->function(3);
	}
	else if (label.compare("f5") == 0) {
		c->function(4);
	}
	else if (label.compare("f6") == 0) {
		c->function(5);
	}
	else if (label.compare("rec") == 0) {
		c->rec();
	}
	else if (label.compare("overdub") == 0) {
		c->overDub();
	}
	else if (label.compare("tap") == 0) {
		c->tap();
	}
	else if (label.compare("go-to") == 0) {
		c->goTo();
	}
}
