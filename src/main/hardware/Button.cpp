#include "Button.hpp"

#include <Mpc.hpp>

#include <lcdgui/ScreenComponent.hpp>

#include <controls/BaseControls.hpp>
#include <controls/GlobalReleaseControls.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::hardware;
using namespace std;

Button::Button(mpc::Mpc& mpc, const string& label)
	: HwComponent(mpc, label)
{
}

void Button::push()
{
	auto ls = mpc.getLayeredScreen().lock();

//	if (mpc.getDisk().lock()->isBusy() && ls->getCurrentScreenName().compare("cant-find-file") != 0)
//		return;

	auto screen = ls->findScreenComponent().lock();
	
	if (!screen)
		return;
	
	if (label.compare("left") == 0) {
		screen->left();
	}
	else if (label.compare("right") == 0) {
		screen->right();
	}
	else if (label.compare("up") == 0) {
		screen->up();
	}
	else if (label.compare("down") == 0) {
		screen->down();
	}
	else if (label.compare("rec") == 0) {
		screen->rec();
	}
	else if (label.compare("overdub") == 0) {
		screen->overDub();
	}
	else if (label.compare("stop") == 0) {
		screen->stop();
	}
	else if (label.compare("play") == 0) {
		screen->play();
	}
	else if (label.compare("play-start") == 0) {
		screen->playStart();
	}
	else if (label.compare("main-screen") == 0) {
		screen->mainScreen();
	}
	else if (label.compare("open-window") == 0) {
		screen->openWindow();
	}
	else if (label.compare("prev-step-event") == 0) {
		screen->prevStepEvent();
	}
	else if (label.compare("next-step-event") == 0) {
		screen->nextStepEvent();
	}
	else if (label.compare("go-to") == 0) {
		screen->goTo();
	}
	else if (label.compare("prev-bar-start") == 0) {
		screen->prevBarStart();
	}
	else if (label.compare("next-bar-end") == 0) {
		screen->nextBarEnd();
	}
	else if (label.compare("tap") == 0) {
		screen->tap();
	}
	else if (label.compare("next-seq") == 0) {
		screen->nextSeq();
	}
	else if (label.compare("track-mute") == 0) {
		screen->trackMute();
	}
	else if (label.compare("full-level") == 0) {
		screen->fullLevel();
	}
	else if (label.compare("sixteen-levels") == 0) {
		screen->sixteenLevels();
	}
	else if (label.compare("f1") == 0) {
		screen->function(0);
	}
	else if (label.compare("f2") == 0) {
		screen->function(1);
	}
	else if (label.compare("f3") == 0) {
		screen->function(2);
	}
	else if (label.compare("f4") == 0) {
		screen->function(3);
	}
	else if (label.compare("f5") == 0) {
		screen->function(4);
	}
	else if (label.compare("f6") == 0) {
		screen->function(5);
	}
	else if (label.compare("shift") == 0) {
		screen->shift();
	}
	else if (label.compare("enter") == 0) {
		screen->pressEnter();
	}
	else if (label.compare("undo-seq") == 0) {
		screen->undoSeq();
	}
	else if (label.compare("erase") == 0) {
		screen->erase();
	}
	else if (label.compare("after") == 0) {
		screen->after();
	}
	else if (label.compare("bank-a") == 0) {
		screen->bank(0);
	}
	else if (label.compare("bank-b") == 0) {
		screen->bank(1);
	}
	else if (label.compare("bank-c") == 0) {
		screen->bank(2);
	}
	else if (label.compare("bank-d") == 0) {
		screen->bank(3);
	}
	else if (label.compare("0") == 0) {
		screen->numpad(0);
	}
	else if (label.compare("1") == 0) {
		screen->numpad(1);
	}
	else if (label.compare("2") == 0) {
		screen->numpad(2);
	}
	else if (label.compare("3") == 0) {
		screen->numpad(3);
	}
	else if (label.compare("4") == 0) {
		screen->numpad(4);
	}
	else if (label.compare("5") == 0) {
		screen->numpad(5);
	}
	else if (label.compare("6") == 0) {
		screen->numpad(6);
	}
	else if (label.compare("7") == 0) {
		screen->numpad(7);
	}
	else if (label.compare("8") == 0) {
		screen->numpad(8);
	}
	else if (label.compare("9") == 0) {
		screen->numpad(9);
	}
}

void Button::release() {
//	if (mpc.getDisk().lock()->isBusy()) return;
	
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
	else if (label.compare("play") == 0) {
		c->play();
	}
	else if (label.compare("tap") == 0) {
		c->tap();
	}
	else if (label.compare("go-to") == 0) {
		c->goTo();
	}
}
