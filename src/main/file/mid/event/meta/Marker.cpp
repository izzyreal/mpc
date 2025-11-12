#include "file/mid/event/meta/Marker.hpp"

#include "file/mid/event/meta/MetaEvent.hpp"

using namespace mpc::file::mid::event::meta;

Marker::Marker(int tick, int delta, const std::string &marker)
    : TextualMetaEvent(tick, delta, MARKER, marker)
{
}

void Marker::setMarkerName(const std::string &name)
{
    setText(name);
}

std::string Marker::getMarkerName()
{
    return getText();
}
