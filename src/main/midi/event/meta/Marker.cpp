#include <midi/event/meta/Marker.hpp>

#include <midi/event/meta/MetaEvent.hpp>

using namespace mpc::midi::event::meta;
using namespace std;

Marker::Marker(int tick, int delta, string marker) : TextualMetaEvent(tick, delta, MetaEvent::MARKER, marker)
{
}

void Marker::setMarkerName(string name)
{
    setText(name);
}

string Marker::getMarkerName()
{
    return getText();
}
