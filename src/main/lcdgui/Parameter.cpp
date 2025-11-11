#include "Parameter.hpp"

#include "Field.hpp"
#include "Label.hpp"

#include "Util.hpp"

using namespace mpc::lcdgui;

Parameter::Parameter(Mpc &mpc, std::string labelStr, std::string name, int x,
                     int y, int fieldWidth)
    : Component(name)
{
    const auto labelWidth = Util::getTextWidthInPixels(labelStr);

    addChild(
        std::make_shared<Label>(mpc, name, labelStr, x, y - 1, labelWidth));

    // We add + 1 to the field width, because currently the json resources
    // provide n_characters * font_width for their width, and it should be + 1
    // to acommodate for the rectangle of a field.
    addChild(
        std::make_shared<Field>(mpc, name, x + labelWidth, y, fieldWidth + 1));
}
