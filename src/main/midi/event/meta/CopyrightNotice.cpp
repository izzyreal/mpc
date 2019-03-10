#include <midi/event/meta/CopyrightNotice.hpp>

#include <midi/event/meta/MetaEvent.hpp>

using namespace mpc::midi::event::meta;
using namespace std;

CopyrightNotice::CopyrightNotice(int tick, int delta, string text) : TextualMetaEvent(tick, delta, MetaEvent::COPYRIGHT_NOTICE, text)
{
}

void CopyrightNotice::setNotice(string t)
{
    setText(t);
}

string CopyrightNotice::getNotice()
{
    return getText();
}
