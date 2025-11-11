#include "Info.hpp"

using namespace mpc::lcdgui;

Info::Info(Mpc &mpc, const std::string &name, int x, int y, int size)
    : Component("info-for-" + name)
{
    addChild(std::make_shared<Label>(mpc, name, "", x, y + 2, size));
}
