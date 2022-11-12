#include <midi/event/meta/TrackName.hpp>

#include <midi/event/meta/MetaEvent.hpp>

using namespace mpc::midi::event::meta;

TrackName::TrackName(int tick, int delta, std::string name) : TextualMetaEvent(tick, delta, MetaEvent::TRACK_NAME, name)
{
}

void TrackName::setName(std::string name)
{
    setText(name);
}

std::string TrackName::getTrackName()
{
    return getText();
}
