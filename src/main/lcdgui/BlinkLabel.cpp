#include <lcdgui/BlinkLabel.hpp>

#include <chrono>

using namespace mpc::lcdgui;
using namespace std;

BlinkLabel::BlinkLabel(const string& name, const string& text, int x, int y, int columns)
	: Label(name, text, x, y, columns)
{
	this->rect = MRECT(133, 52, 158, 58);
	setText(text);
}

void BlinkLabel::static_blink(void * args)
{
	static_cast<BlinkLabel*>(args)->runBlinkThread();
}

void BlinkLabel::Hide(bool b) {
	Component::Hide(b);
}

void BlinkLabel::runBlinkThread() {
	while (blinking) {
		int counter = 0;
		while (blinking && counter++ != BLINK_INTERVAL) {
			this_thread::sleep_for(chrono::milliseconds(1));
		}
		BlinkLabel::Hide(!IsHidden());
		SetDirty();
	}
	Hide(false);
}

void BlinkLabel::setBlinking(bool flag)
{
    this->blinking = flag;
	if (blinking) {
		if (blinkThread.joinable()) blinkThread.join();
		blinkThread = thread(&BlinkLabel::static_blink, this);
	}
}

bool BlinkLabel::isBlinking()
{
    return blinking;
}

BlinkLabel::~BlinkLabel() {
	if (blinking) {
		blinking = false;
		//blinkThread.detach();
	}
	if (blinkThread.joinable()) blinkThread.join();
}
