#include "Parameter.hpp"

#include "Field.hpp"
#include "Label.hpp"

#include <Util.hpp>

using namespace mpc::lcdgui;
using namespace std;

Parameter::Parameter(mpc::Mpc& mpc, string labelStr, string name, int x, int y, int fieldWidth)
	: Component(name)
{
	const auto labelWidth = mpc::Util::getTextWidthInPixels(labelStr);
	
	addChild(make_shared<Label>(mpc, name, labelStr, x, y - 1, labelWidth));
	
	// We add + 1 to the field width, because currently the json resources provide
	// n_characters * font_width for their width, and it should be + 1 to acommodate
	// for the rectangle of a field.
	addChild(make_shared<Field>(mpc, name, x + labelWidth, y, fieldWidth + 1));
}
