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

bool Controls::isRecPressed(bool includeLocked)
{
	return recPressed || (includeLocked && recLocked);
}

bool Controls::isOverdubPressed(bool includeLocked)
{
	return overDubPressed || (includeLocked && overDubLocked);
}

bool Controls::isNoteRepeatLocked()
{
    return noteRepeatLocked;
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

void Controls::setNoteRepeatLocked(bool b)
{
    noteRepeatLocked = b;
}

std::weak_ptr<KbMapping> Controls::getKbMapping()
{
    return kbMapping;
}

