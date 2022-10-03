#include "Controls.hpp"

#include "KeyEventHandler.hpp"

#include "KbMapping.hpp"

#include <Mpc.hpp>

#include <controls/BaseControls.hpp>

#include <controls/GlobalReleaseControls.hpp>

using namespace mpc::controls;
using namespace std;

Controls::Controls(mpc::Mpc& _mpc)
	: controls (make_shared<BaseControls>(_mpc)),
	releaseControls (make_shared<GlobalReleaseControls>(_mpc)),
	keyEventHandler (make_shared<KeyEventHandler>(_mpc)),
	kbMapping (make_shared<KbMapping>())
{
}

weak_ptr<KeyEventHandler> Controls::getKeyEventHandler()
{
    return keyEventHandler;
}

void Controls::setCtrlPressed(bool b)
{
	ctrlPressed = b;
}

void Controls::setAltPressed(bool b)
{
	altPressed = b;
}

bool Controls::isCtrlPressed()
{
	return ctrlPressed;
}

bool Controls::isAltPressed()
{
	return altPressed;
}

bool Controls::isErasePressed()
{
	return erasePressed;
}

bool Controls::isRecPressed()
{
	return recPressed;
}

bool Controls::isOverDubPressed()
{
	return overDubPressed;
}

bool Controls::isTapPressed()
{
	return tapPressed;
}

bool Controls::isNoteRepeatLocked()
{
    return noteRepeatLocked;
}

bool Controls::isGoToPressed()
{
	return goToPressed;
}

bool Controls::isShiftPressed()
{
	return shiftPressed;
}

bool Controls::isF3Pressed()
{
	return f3Pressed;
}

bool Controls::isF4Pressed()
{
	return f4Pressed;
}

bool Controls::isF5Pressed()
{
	return f5Pressed;
}

bool Controls::isF6Pressed()
{
	return f6Pressed;
}

void Controls::setErasePressed(bool b)
{
	erasePressed = b;
}

void Controls::setRecPressed(bool b)
{
	recPressed = b;
}

void Controls::setOverDubPressed(bool b)
{
	overDubPressed = b;
}

void Controls::setTapPressed(bool b)
{
	tapPressed = b;
}

void Controls::setNoteRepeatLocked(bool b)
{
    noteRepeatLocked = b;
}

void Controls::setGoToPressed(bool b)
{
	goToPressed = b;
}

void Controls::setShiftPressed(bool b)
{
	shiftPressed = b;
}

void Controls::setF3Pressed(bool b)
{
	f3Pressed = b;
}

void Controls::setF4Pressed(bool b)
{
	f4Pressed = b;
}

void Controls::setF5Pressed(bool b)
{
	f5Pressed = b;
}

void Controls::setF6Pressed(bool b)
{
	f6Pressed = b;
}

shared_ptr<BaseControls> Controls::getControls()
{
	return controls;
}

shared_ptr<GlobalReleaseControls> Controls::getReleaseControls()
{
	return releaseControls;
}

weak_ptr<KbMapping> Controls::getKbMapping()
{
    return kbMapping;
}

void Controls::setPlayPressed(bool b)
{
    playPressed = b;
}

bool Controls::isPlayPressed() {
    return playPressed;
}
