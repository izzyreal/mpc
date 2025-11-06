#include "file/mid/event/meta/InstrumentName.hpp"

#include "file/mid/event/meta/MetaEvent.hpp"

using namespace mpc::file::mid::event::meta;

InstrumentName::InstrumentName(int tick, int delta, const std::string &name)
    : TextualMetaEvent(tick, delta, MetaEvent::INSTRUMENT_NAME, name)
{
}

void InstrumentName::setName(const std::string &name)
{
    setText(name);
}

std::string InstrumentName::getName()
{
    return getText();
}
