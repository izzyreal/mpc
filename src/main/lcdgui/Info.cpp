#include "Info.hpp"

using namespace mpc::lcdgui;
using namespace std;

Info::Info(mpc::Mpc& mpc, const string& name, int x, int y, int size)
	: Component("info-for-" + name)
{
	addChild(make_shared<Label>(mpc, name, "", x, y + 2, size));
}
