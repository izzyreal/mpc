#include <midi/event/meta/Marker.hpp>

#include <midi/event/meta/MetaEvent.hpp>

using namespace mpc::midi::event::meta;

Marker::Marker(int tick, int delta, std::string marker) : TextualMetaEvent(tick, delta, MetaEvent::MARKER, marker)
{
}

void Marker::setMarkerName(std::string name)
{
    setText(name);
}

std::string Marker::getMarkerName()
{
    return getText();
}
