#include <midi/event/meta/TrackName.hpp>

#include <midi/event/meta/MetaEvent.hpp>

using namespace mpc::midi::event::meta;
using namespace std;

TrackName::TrackName(int tick, int delta, string name) : TextualMetaEvent(tick, delta, MetaEvent::TRACK_NAME, name)
{
}

void TrackName::setName(string name)
{
    setText(name);
}

string TrackName::getTrackName()
{
    return getText();
}
