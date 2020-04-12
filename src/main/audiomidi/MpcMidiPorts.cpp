#include "MpcMidiPorts.hpp"

#include <midi/core/ShortMessage.hpp>

using namespace mpc::audiomidi;
using namespace std;

MpcMidiPorts::MpcMidiPorts()
{
	receivers = vector<vector<ctoot::midi::core::ShortMessage>>(2);
}

vector<vector<ctoot::midi::core::ShortMessage>>& MpcMidiPorts::getReceivers() {
	return receivers;
}

MpcMidiPorts::~MpcMidiPorts() {
}
