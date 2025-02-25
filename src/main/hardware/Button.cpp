#include "Button.hpp"

#include <Mpc.hpp>

#include <lcdgui/ScreenComponent.hpp>

#include <controls/BaseControls.hpp>
#include <controls/GlobalReleaseControls.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::hardware;

Button::Button(mpc::Mpc& mpc, const std::string& label)
	: HwComponent(mpc, label)
{
}

void Button::push()
{
    HwComponent::push();

	auto ls = mpc.getLayeredScreen();

	auto screen = ls->findScreenComponent();
	
	if (!screen)
		return;
	
	if (label == "left") {
		screen->left();
	}
	else if (label == "right") {
		screen->right();
	}
	else if (label == "up") {
		screen->up();
	}
	else if (label == "down") {
		screen->down();
	}
	else if (label == "rec") {
		screen->rec();
	}
	else if (label == "overdub") {
		screen->overDub();
	}
	else if (label == "stop") {
		screen->stop();
	}
	else if (label == "play") {
		screen->play();
	}
	else if (label == "play-start") {
		screen->playStart();
	}
	else if (label == "main-screen") {
		screen->mainScreen();
	}
	else if (label == "open-window") {
		screen->openWindow();
	}
	else if (label == "prev-step-event") {
		screen->prevStepEvent();
	}
	else if (label == "next-step-event") {
		screen->nextStepEvent();
	}
	else if (label == "go-to") {
		screen->goTo();
	}
	else if (label == "prev-bar-start") {
		screen->prevBarStart();
	}
	else if (label == "next-bar-end") {
		screen->nextBarEnd();
	}
	else if (label == "tap") {
		screen->tap();
	}
	else if (label == "next-seq") {
		screen->nextSeq();
	}
	else if (label == "track-mute") {
		screen->trackMute();
	}
	else if (label == "full-level") {
		screen->fullLevel();
	}
	else if (label == "sixteen-levels") {
		screen->sixteenLevels();
	}
	else if (label == "f1") {
		screen->function(0);
	}
	else if (label == "f2") {
		screen->function(1);
	}
	else if (label == "f3") {
		screen->function(2);
	}
	else if (label == "f4") {
		screen->function(3);
	}
	else if (label == "f5") {
		screen->function(4);
	}
	else if (label == "f6") {
		screen->function(5);
	}
	else if (label == "shift" || label == "shift_#1" || label == "shift_#2" || label == "shift_#3") {
		screen->shift();
	}
	else if (label == "enter") {
		screen->pressEnter();
	}
	else if (label == "undo-seq") {
		screen->undoSeq();
	}
	else if (label == "erase") {
		screen->erase();
	}
	else if (label == "after") {
		screen->after();
	}
	else if (label == "bank-a") {
		screen->bank(0);
	}
	else if (label == "bank-b") {
		screen->bank(1);
	}
	else if (label == "bank-c") {
		screen->bank(2);
	}
	else if (label == "bank-d") {
		screen->bank(3);
	}
	else if (label == "0" || label == "0 (extra)") {
		screen->numpad(0);
	}
	else if (label == "1" || label == "1 (extra)") {
		screen->numpad(1);
	}
	else if (label == "2" || label == "2 (extra)") {
		screen->numpad(2);
	}
	else if (label == "3" || label == "3 (extra)") {
		screen->numpad(3);
	}
	else if (label == "4" || label == "4 (extra)") {
		screen->numpad(4);
	}
	else if (label == "5" || label == "5 (extra)") {
		screen->numpad(5);
	}
	else if (label == "6" || label == "6 (extra)") {
		screen->numpad(6);
	}
	else if (label == "7" || label == "7 (extra)") {
		screen->numpad(7);
	}
	else if (label == "8" || label == "8 (extra)") {
		screen->numpad(8);
	}
	else if (label == "9" || label == "9 (extra)") {
		screen->numpad(9);
	}
}

void Button::release() {
    auto c = mpc.getReleaseControls();
	
	if (label == "shift" || label == "shift_#1" || label == "shift_#2" || label == "shift_#3") {
		c->shift();
	}
	else if (label == "erase") {
		c->erase();
	}
	else if (label == "f1") {
		c->function(0);
	}
	else if (label == "f3") {
		c->function(2);
	}
	else if (label == "f4") {
		c->function(3);
	}
	else if (label == "f5") {
		c->function(4);
	}
	else if (label == "f6") {
		c->function(5);
	}
	else if (label == "rec") {
		c->rec();
	}
	else if (label == "overdub") {
		c->overDub();
	}
	else if (label == "play") {
		c->play();
	}
	else if (label == "tap") {
		c->tap();
	}
	else if (label == "go-to") {
		c->goTo();
	}
}
