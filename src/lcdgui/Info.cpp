#include "Info.hpp"

using namespace mpc::lcdgui;
using namespace std;

Info::Info(weak_ptr<Label> label, string name, int x, int y, int size) {
	this->label = label;
	label.lock()->initialize(name, " ", x, y, size);
	label.lock()->enableRigorousClearing();
}

weak_ptr<Label> Info::getLabel() {
	return label;
}

Info::~Info() {
}
