#include "file/mid/event/meta/CuePoint.hpp"

#include "file/mid/event/meta/MetaEvent.hpp"

using namespace mpc::file::mid::event::meta;

CuePoint::CuePoint(int tick, int delta, const std::string &marker)
    : TextualMetaEvent(tick, delta, CUE_POINT, marker)
{
}

void CuePoint::setCue(const std::string &name)
{
    setText(name);
}

std::string CuePoint::getCue()
{
    return getText();
}
