#include "Parameter.hpp"
#include "Field.hpp"
#include "Label.hpp"

using namespace mpc::lcdgui;
using namespace std;

Parameter::Parameter(string labelStr, string name, int x, int y, int tfColumns)
	: Component(name)
{
	label = make_shared<Label>(name, labelStr, x, y - 1, labelStr.size());

	const char* p = labelStr.c_str();
	int count = 0;
	for (p; *p != 0; ++p)
		count += ((*p & 0xc0) != 0x80);
	int tfOffset = (count * 6);

	tf = make_shared<Field>(name, x + tfOffset, y - 1, tfColumns);
}

void Parameter::Draw(std::vector<std::vector<bool>>* pixels)
{
	label->Draw(pixels);
	tf->Draw(pixels);
}

weak_ptr<Field> Parameter::getTf() {
	return tf;
}

weak_ptr<Label> Parameter::getLabel() {
	return label;
}

Parameter::~Parameter() {
}
