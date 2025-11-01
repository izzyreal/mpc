#include "file/mid/event/meta/CuePoint.hpp"

#include "file/mid/event/meta/MetaEvent.hpp"

using namespace mpc::file::mid::event::meta;

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
