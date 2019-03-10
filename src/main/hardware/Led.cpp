#include "Led.hpp"

using namespace mpc::hardware;
using namespace std;

Led::Led(string label)
{
	this->label = label;
}

string Led::getLabel() {
	return label;
}

void Led::light(bool b) {
	if (b) {
		notifyObservers(string(label + "on"));
	}
	else {
		notifyObservers(string(label + "off"));
	}
}

Led::~Led() {
}
