#include "Parameter.hpp"
#include "Field.hpp"
#include "Label.hpp"

using namespace mpc::lcdgui;
using namespace std;

Parameter::Parameter(weak_ptr<Field> tf, std::weak_ptr<Label> label, string labelStr, string name, int x, int y, int tfColumns) {
	this->label = label;
	auto lLabel = label.lock();
	lLabel->initialize(name, labelStr, x, y - 1, labelStr.size());

	const char* p = labelStr.c_str();
	int count = 0;
	for (p; *p != 0; ++p)
		count += ((*p & 0xc0) != 0x80);
	int tfOffset = (count * 6);
	this->tf = tf;
	auto lTf = tf.lock();
	lTf->initialize(name, x + tfOffset, y - 1, tfColumns);
}

weak_ptr<Field> Parameter::getTf() {
	return tf;
}

weak_ptr<Label> Parameter::getLabel() {
	return label;
}

Parameter::~Parameter() {
}
