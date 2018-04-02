#include <ui/misc/TransGui.hpp>

using namespace mpc::ui::misc;
using namespace std;

TransGui::TransGui()
{
}

void TransGui::setAmount(int i)
{
	if (i < -12 || i > 12) return;
	amount = i;
	setChanged();
	notifyObservers(string("amount"));
}

int TransGui::getAmount()
{
    return amount;
}

void TransGui::setTr(int i)
{
    if(i < -1 || i > 63) return;
    tr = i;
    setChanged();
    notifyObservers(string("tr"));
}

int TransGui::getTr()
{
    return tr;
}

void TransGui::setBar0(int i)
{
    if(i < 0) return;
    bar0 = i;
    if(bar0 > bar1) bar1 = bar0;
    setChanged();
    notifyObservers(string("bars"));
}

int TransGui::getBar0()
{
    return bar0;
}

void TransGui::setBar1(int i)
{
    if(i < 0) return;
    bar1 = i;
    if(bar1 < bar0) bar0 = bar1;
    setChanged();
    notifyObservers(string("bars"));
}

int TransGui::getBar1()
{
    return bar1;
}
