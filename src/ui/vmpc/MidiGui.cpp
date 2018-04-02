#include <ui/vmpc/MidiGui.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

MidiGui::MidiGui()
{
}

void MidiGui::setIn(int i)
{
    if (i < 0 || i > 1) return;
    in = i;
    setChanged();
	notifyObservers(string("in"));
}

int MidiGui::getIn()
{
    return in;
}

void MidiGui::setOut(int i)
{
    if (i < 0 || i > 1)
        return;

    out = i;
    setChanged();
    notifyObservers(string("out"));
}

int MidiGui::getOut()
{
    return out;
}

void MidiGui::setIn1TransmitterNumber(int i, int max)
{
	if (i < -1 || i > max)
		return;

	auto difference = i - in1TransmitterNumber;
	if (i == in2TransmitterNumber && in2TransmitterNumber != -1) {
		if (difference < 0 && i + difference < -1)
			return;

		if (difference > 0 && i + difference > max)
			return;

		i += difference;
	}
	in1TransmitterNumber = i;
	setChanged();
	notifyObservers(string("dev0"));
}

int MidiGui::getIn1TransmitterNumber()
{
    return in1TransmitterNumber;
}

void MidiGui::setIn2TransmitterNumber(int i, int max)
{
	if (i < -1 || i > max) return;

	auto difference = i - in2TransmitterNumber;
	if (i == in1TransmitterNumber && in1TransmitterNumber != -1) {
		if (difference < 0 && i + difference < -1)
			return;

		if (difference > 0 && i + difference > max)
			return;

		i += (difference);
	}
	in2TransmitterNumber = i;
	setChanged();
	notifyObservers(string("dev0"));
}

int MidiGui::getIn2TransmitterNumber()
{
    return in2TransmitterNumber;
}

void MidiGui::setOutAReceiverNumber(int i, int max)
{
	if (i < -1 || i > max) return;

	auto difference = i - outAReceiverNumber;
	if (i == outBReceiverNumber && outBReceiverNumber != -1) {
		if (difference < 0 && i + difference < -1)
			return;

		if (difference > 0 && i + difference > max)
			return;

		i += (difference);
	}
	outAReceiverNumber = i;
//	invokeLater(new MidiGui_setOutAReceiverNumber_3(this));
}

int MidiGui::getOutAReceiverIndex()
{
    return outAReceiverNumber;
}

void MidiGui::setOutBReceiverNumber(int i, int max)
{
	if (i < -1 || i > max)
		return;

	auto difference = i - outBReceiverNumber;
	if (i == outAReceiverNumber && outAReceiverNumber != -1) {
		if (difference < 0 && i + difference < -1)
			return;

		if (difference > 0 && i + difference > max)
			return;

		i += (difference);
	}
	outBReceiverNumber = i;
//	invokeLater(new MidiGui_setOutBReceiverNumber_4(this));
}

int MidiGui::getOutBReceiverIndex()
{
    return outBReceiverNumber;
}
