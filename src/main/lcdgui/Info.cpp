#include "Info.hpp"

using namespace mpc::lcdgui;
using namespace std;

Info::Info(const string& name, int x, int y, int size)
	: Component("info-for-" + name)
{
	auto label = dynamic_pointer_cast<Label>(addChild(make_shared<Label>(name, "", x, y - 1, 0)).lock());
	label->enableRigorousClearing();
}

Info::~Info() {
}
