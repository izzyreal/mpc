#include "Info.hpp"

using namespace mpc::lcdgui;
using namespace std;

Info::Info(const string& name, int x, int y, int size)
	: Component("info-for-" + name)
{
	label = make_shared<Label>(name, " ", x, y, size);
	label->enableRigorousClearing();
}

weak_ptr<Label> Info::getLabel() {
	return label;
}

void Info::Draw(std::vector<std::vector<bool>>* pixels)
{
	label->Draw(pixels);
}

Info::~Info() {
}
