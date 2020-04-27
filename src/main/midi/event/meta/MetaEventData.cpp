#include <midi/event/meta/MetaEventData.hpp>

using namespace mpc::midi::event::meta;
using namespace std;

MetaEventData::MetaEventData(stringstream& in)
{
	type = in.get();
	
	length = mpc::midi::util::VariableLengthInt(in);
	
	data = vector<char>(length.getValue());

	if (length.getValue() > 0) {
		in.read(&data[0], data.size());
	}
}
