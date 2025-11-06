#include "file/mid/event/meta/TrackName.hpp"

#include "file/mid/event/meta/MetaEvent.hpp"

using namespace mpc::file::mid::event::meta;

TrackName::TrackName(int tick, int delta, const std::string &name)
    : TextualMetaEvent(tick, delta, MetaEvent::TRACK_NAME, name)
{
}

void TrackName::setName(const std::string &name)
{
    setText(name);
}

std::string TrackName::getTrackName()
{
    return getText();
}
