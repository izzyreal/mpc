#include "Button.hpp"

#include <Mpc.hpp>

#include <controls/AbstractControls.hpp>
#include <controls/GlobalReleaseControls.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::hardware;
using namespace std;

Button::Button(mpc::Mpc* mpc, string label)
{
	this->mpc = mpc;
	this->label = label;
}

string Button::getLabel() {
	return label;
}

void Button::push() {
	if (mpc->getDisk().lock()->isBusy()) return;
	auto c = mpc->getActiveControls();
	if (!c) return;
	if (label.compare("left") == 0) {
		c->left();
	}
	else if (label.compare("right") == 0) {
		c->right();
	}
	else if (label.compare("up") == 0) {
		c->up();
	}
	else if (label.compare("down") == 0) {
		c->down();
	}
	else if (label.compare("rec") == 0) {
		c->rec();
	}
	else if (label.compare("overdub") == 0) {
		c->overDub();
	}
	else if (label.compare("stop") == 0) {
		c->stop();
	}
	else if (label.compare("play") == 0) {
		c->play();
	}
	else if (label.compare("playstart") == 0) {
		c->playStart();
	}
	else if (label.compare("mainscreen") == 0) {
		c->mainScreen();
	}
	else if (label.compare("openwindow") == 0) {
		c->openWindow();
	}
	else if (label.compare("prevstepevent") == 0) {
		c->prevStepEvent();
	}
	else if (label.compare("nextstepevent") == 0) {
		c->nextStepEvent();
	}
	else if (label.compare("goto") == 0) {
		c->goTo();
	}
	else if (label.compare("prevbarstart") == 0) {
		c->prevBarStart();
	}
	else if (label.compare("nextbarend") == 0) {
		c->nextBarEnd();
	}
	else if (label.compare("tap") == 0) {
		c->tap();
	}
	else if (label.compare("nextseq") == 0) {
		c->nextSeq();
	}
	else if (label.compare("trackmute") == 0) {
		c->trackMute();
	}
	else if (label.compare("fulllevel") == 0) {
		c->fullLevel();
	}
	else if (label.compare("sixteenlevels") == 0) {
		c->sixteenLevels();
	}
	else if (label.compare("f1") == 0) {
		c->function(0);
	}
	else if (label.compare("f2") == 0) {
		c->function(1);
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
	else if (label.compare("shift") == 0) {
		c->shift();
	}
	else if (label.compare("enter") == 0) {
		c->pressEnter();
	}
	else if (label.compare("undoseq") == 0) {
		c->undoSeq();
	}
	else if (label.compare("erase") == 0) {
		c->erase();
	}
	else if (label.compare("after") == 0) {
		c->after();
	}
	else if (label.compare("banka") == 0) {
		c->bank(0);
	}
	else if (label.compare("bankb") == 0) {
		c->bank(1);
	}
	else if (label.compare("bankc") == 0) {
		c->bank(2);
	}
	else if (label.compare("bankd") == 0) {
		c->bank(3);
	}
	else if (label.compare("0") == 0) {
		c->numpad(0);
	}
	else if (label.compare("1") == 0) {
		c->numpad(1);
	}
	else if (label.compare("2") == 0) {
		c->numpad(2);
	}
	else if (label.compare("3") == 0) {
		c->numpad(3);
	}
	else if (label.compare("4") == 0) {
		c->numpad(4);
	}
	else if (label.compare("5") == 0) {
		c->numpad(5);
	}
	else if (label.compare("6") == 0) {
		c->numpad(6);
	}
	else if (label.compare("7") == 0) {
		c->numpad(7);
	}
	else if (label.compare("8") == 0) {
		c->numpad(8);
	}
	else if (label.compare("9") == 0) {
		c->numpad(9);
	}
}

void Button::release() {
	if (mpc->getDisk().lock()->isBusy()) return;
	auto c = mpc->getReleaseControls();
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
}

Button::~Button() {
}
