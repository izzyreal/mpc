#include "midi/event/meta/Text.hpp"

#include "midi/event/meta/MetaEvent.hpp"

using namespace mpc::midi::event::meta;

Text::Text(int tick, int delta, std::string text)
    : TextualMetaEvent(tick, delta, MetaEvent::TEXT_EVENT, text)
{
}
