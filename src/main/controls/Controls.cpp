#include "Controls.hpp"

#include "KbMapping.hpp"

#include <Mpc.hpp>

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>

using namespace mpc::controls;

Controls::Controls(mpc::Mpc& _mpc) : 
	mpc(_mpc),
	kbMapping (std::make_shared<KbMapping>(_mpc))
{
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

bool Controls::isRecPressed(bool includeLocked)
{
	return recPressed || (includeLocked && recLocked);
}

bool Controls::isOverdubPressed(bool includeLocked)
{
	return overDubPressed || (includeLocked && overDubLocked);
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

bool mpc::controls::Controls::isStepRecording()
{
	bool posIsLastTick = mpc.getSequencer()->getTickPosition() == mpc.getSequencer()->getActiveSequence()->getLastTick();
	auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();
	bool step = currentScreenName == "step-editor" && !posIsLastTick;
	return step;
}

bool mpc::controls::Controls::isRecMainWithoutPlaying()
{
    auto sequencer = mpc.getSequencer();
	auto tc_note = mpc.screens->get<mpc::lcdgui::screens::window::TimingCorrectScreen>("timing-correct")->getNoteValue();
	bool posIsLastTick = sequencer->getTickPosition() == sequencer->getActiveSequence()->getLastTick();
	auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();
	bool recMainWithoutPlaying = currentScreenName == "sequencer" &&
		!sequencer->isPlaying() &&
		isRecPressed() &&
		tc_note != 0 &&
		!posIsLastTick;
	return recMainWithoutPlaying;
}

void Controls::setErasePressed(bool b)
{
	erasePressed = b;
}

void Controls::setRecPressed(bool b)
{
    recPressed = b;
}

void Controls::setRecLocked(bool b)
{
	recLocked = b;
}

void Controls::setOverdubPressed(bool b)
{
	overDubPressed = b;
}

void Controls::setOverdubLocked(bool b)
{
	overDubLocked = b;
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

std::weak_ptr<KbMapping> Controls::getKbMapping()
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
