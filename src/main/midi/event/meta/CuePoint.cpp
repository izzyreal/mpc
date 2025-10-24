#include "midi/event/meta/CuePoint.hpp"

#include "midi/event/meta/MetaEvent.hpp"

using namespace mpc::midi::event::meta;

CuePoint::CuePoint(int tick, int delta, std::string marker)
: TextualMetaEvent(tick, delta, MetaEvent::CUE_POINT, marker)
{
}

void CuePoint::setCue(std::string name)
{
    setText(name);
}

std::string CuePoint::getCue()
{
    return getText();
}
