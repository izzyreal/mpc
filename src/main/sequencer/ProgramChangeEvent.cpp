#include <sequencer/ProgramChangeEvent.hpp>

using namespace mpc::sequencer;

void ProgramChangeEvent::setProgram(int i)
{
	if (i < 1 || i > 128) return;
	programChangeValue = i;
	
	notifyObservers(std::string("step-editor"));
}

int ProgramChangeEvent::getProgram()
{
    return programChangeValue;
}

void ProgramChangeEvent::CopyValuesTo(std::weak_ptr<Event> dest) {
	Event::CopyValuesTo(dest);
	auto lDest = std::dynamic_pointer_cast<ProgramChangeEvent>(dest.lock());
	lDest->setProgram(getProgram());
}
