#include <ui/misc/PunchGui.hpp>

using namespace mpc::ui::misc;
using namespace std;

PunchGui::PunchGui()
{
}

void PunchGui::setAutoPunch(int i)
{
	if (i < 0 || i > 2) return;
	autoPunch = i;
	setChanged();
	notifyObservers(string("autopunch"));
}

int PunchGui::getAutoPunch()
{
    return autoPunch;
}

void PunchGui::setTime0(int l)
{
    time0 = l;
    if(time0 > time1) time1 = time0;
    setChanged();
    notifyObservers(string("time"));
}

int PunchGui::getTime0()
{
    return time0;
}

void PunchGui::setTime1(int l)
{
    time1 = l;
    if(time1 < time0) time0 = time1;
    setChanged();
    notifyObservers(string("time"));
}

int PunchGui::getTime1()
{
    return time1;
}
