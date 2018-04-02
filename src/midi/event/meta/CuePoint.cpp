#include <midi/event/meta/CuePoint.hpp>

#include <midi/event/meta/MetaEvent.hpp>

using namespace mpc::midi::event::meta;
using namespace std;

CuePoint::CuePoint(int tick, int delta, string marker) : TextualMetaEvent(tick, delta, MetaEvent::CUE_POINT, marker)
{
}

void CuePoint::setCue(string name)
{
    setText(name);
}

string CuePoint::getCue()
{
    return getText();
}
