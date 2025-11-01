#include "file/mid/event/meta/Text.hpp"

#include "file/mid/event/meta/MetaEvent.hpp"

using namespace mpc::file::mid::event::meta;

Text::Text(int tick, int delta, std::string text)
    : TextualMetaEvent(tick, delta, MetaEvent::TEXT_EVENT, text)
{
}
