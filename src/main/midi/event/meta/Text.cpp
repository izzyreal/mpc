#include <midi/event/meta/Text.hpp>

#include <midi/event/meta/MetaEvent.hpp>

using namespace mpc::midi::event::meta;
using namespace std;

Text::Text(int tick, int delta, string text)
	: TextualMetaEvent(tick, delta, MetaEvent::TEXT_EVENT, text)
{
}
