#include <midi/event/meta/MetaEventData.hpp>

using namespace mpc::midi::event::meta;
using namespace std;

MetaEventData::MetaEventData(moduru::io::InputStream* in)
{
	type = in->read();
	length = new mpc::midi::util::VariableLengthInt(in);
	data = vector<char>(length->getValue());
	if (length->getValue() > 0) {
		in->read(&data);
	}
}
