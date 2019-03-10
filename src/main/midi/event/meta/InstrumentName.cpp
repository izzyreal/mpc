#include <midi/event/meta/InstrumentName.hpp>

#include <midi/event/meta/MetaEvent.hpp>

using namespace mpc::midi::event::meta;
using namespace std;

InstrumentName::InstrumentName(int tick, int delta, string name) : TextualMetaEvent(tick, delta, MetaEvent::INSTRUMENT_NAME, name)
{
}

void InstrumentName::setName(string name)
{
    setText(name);
}

string InstrumentName::getName()
{
    return getText();
}
