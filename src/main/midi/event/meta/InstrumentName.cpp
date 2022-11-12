#include <midi/event/meta/InstrumentName.hpp>

#include <midi/event/meta/MetaEvent.hpp>

using namespace mpc::midi::event::meta;

InstrumentName::InstrumentName(int tick, int delta, std::string name)
: TextualMetaEvent(tick, delta, MetaEvent::INSTRUMENT_NAME, name)
{
}

void InstrumentName::setName(std::string name)
{
    setText(name);
}

std::string InstrumentName::getName()
{
    return getText();
}
