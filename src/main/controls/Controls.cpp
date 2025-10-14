#include "Controls.hpp"

#include "KbMapping.hpp"
#include "controller/ClientInputControllerBase.h"
#include "hardware2/Hardware2.h"

#include <Mpc.hpp>

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>

using namespace mpc::controls;

Controls::Controls(mpc::Mpc& _mpc) : 
	mpc(_mpc),
	kbMapping (std::make_shared<KbMapping>(_mpc))
{
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

    const bool recIsPressedOrLocked = mpc.getHardware2()->getButton("rec")->isPressed() || mpc.inputController->buttonLockTracker.isLocked("rec");

	bool recMainWithoutPlaying = currentScreenName == "sequencer" &&
		!sequencer->isPlaying() &&
		recIsPressedOrLocked &&
		tc_note != 0 &&
		!posIsLastTick;
	return recMainWithoutPlaying;
}

void Controls::setNoteRepeatLocked(bool b)
{
    noteRepeatLocked = b;
}

std::weak_ptr<KbMapping> Controls::getKbMapping()
{
    return kbMapping;
}

