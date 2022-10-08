#include "TopPanel.hpp"

using namespace mpc::hardware;
using namespace std;

bool TopPanel::isAfterEnabled()
{
    return afterEnabled;
}

void TopPanel::setAfterEnabled(bool b)
{
    afterEnabled = b;
}

bool TopPanel::isFullLevelEnabled()
{
    return fullLevelEnabled;
}

void TopPanel::setFullLevelEnabled(bool b)
{
    fullLevelEnabled = b;
}

bool TopPanel::isSixteenLevelsEnabled()
{
    return sixteenLevelsEnabled;
}

void TopPanel::setSixteenLevelsEnabled(bool b)
{
    sixteenLevelsEnabled = b;
}
