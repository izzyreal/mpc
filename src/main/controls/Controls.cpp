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

bool mpc::controls::Controls::isStepRecording()
{
	bool posIsLastTick = mpc.getSequencer()->getTickPosition() == mpc.getSequencer()->getActiveSequence()->getLastTick();
	auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();
	bool step = currentScreenName == "step-editor" && !posIsLastTick;
	return step;
}

std::weak_ptr<KbMapping> Controls::getKbMapping()
{
    return kbMapping;
}

