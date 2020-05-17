#include "Parameter.hpp"

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
	{
		count += ((*p & 0xc0) != 0x80);
	}

	int tfOffset = (count * 6);

	auto field = make_shared<Field>(name, x + tfOffset, y, fieldWidth);
	auto rect = field->getRect();
	rect.L -= 1;
	rect.T -= 1;
	rect.B += 1;

	addChild(field);
}
