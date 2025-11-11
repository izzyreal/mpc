#include "file/mid/event/meta/Text.hpp"

#include "file/mid/event/meta/MetaEvent.hpp"

using namespace mpc::file::mid::event::meta;

Text::Text(int tick, int delta, const std::string &text)
    : TextualMetaEvent(tick, delta, TEXT_EVENT, text)
{
}
