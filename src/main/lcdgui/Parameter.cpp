#include "Parameter.hpp"

#include "Field.hpp"
#include "Label.hpp"

using namespace mpc::lcdgui;
using namespace std;

Parameter::Parameter(string labelStr, string name, int x, int y, int fieldWidth)
	: Component(name)
{
	int count = 0;

	const char* p = labelStr.c_str();

	for (p; *p != 0; ++p)
	{
		count += (*p & 0xc0) != 0x80;
	}

	int halfSpaceCount = 0;

	const string halfSpace = u8"\u00CE";

	int nPos = labelStr.find(halfSpace, 0);

	while (nPos != string::npos)
	{
		halfSpaceCount++;
		nPos = labelStr.find(halfSpace, nPos + halfSpace.size());
	}
	
	const auto labelWidth = (count * 6) - (halfSpaceCount * 3);
	
	addChild(make_shared<Label>(name, labelStr, x, y - 1, labelWidth - 1));
	
	// We add + 1 to the field width, because currently the json resources provide
	// n_characters * font_width for their width, and it should be + 1 to acommodate
	// for the rectangle of a field.
	auto field = make_shared<Field>(name, x + labelWidth, y, fieldWidth + 1);
	auto rect = field->getRect();
	rect.L -= 1;
	rect.T -= 1;
	rect.B += 1;

	addChild(field);
}
