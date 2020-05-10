#include "Parameter.hpp"

#include "Rectangle.hpp"
#include "Field.hpp"
#include "Label.hpp"

using namespace mpc::lcdgui;
using namespace std;

Parameter::Parameter(string labelStr, string name, int x, int y, int fieldWidth)
	: Component(name)
{

	addChild(make_shared<Label>(name, labelStr, x, y, labelStr.size() * 6));

	const char* p = labelStr.c_str();
	int count = 0;
	for (p; *p != 0; ++p)
		count += ((*p & 0xc0) != 0x80);
	int tfOffset = (count * 6);

	auto field = make_shared<Field>(name, x + tfOffset, y, fieldWidth);
	auto rect = field->getRect();
	rect.L -= 1;
	rect.T -= 1;
	rect.B += 1;

	addChild(make_shared<Rectangle>(rect));

	addChild(field);
}

void Parameter::Draw(std::vector<std::vector<bool>>* pixels)
{
	Component::Draw(pixels);
}

void Parameter::takeFocus()
{
	dynamic_pointer_cast<Rectangle>(findChild("rectangle").lock())->setOn(true);
	findField(name).lock()->takeFocus("");
}

void Parameter::loseFocus()
{
	dynamic_pointer_cast<Rectangle>(findChild("rectangle").lock())->setOn(false);
	findField(name).lock()->loseFocus("");
}