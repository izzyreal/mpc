#include <ui/misc/SecondSeqGui.hpp>

using namespace mpc::ui::misc;
using namespace std;

SecondSeqGui::SecondSeqGui()
{
}

void SecondSeqGui::setSq(int i)
{
	if (i < 0 || i > 99) return;
	sq = i;
	setChanged();
	notifyObservers(string("sq"));
}

int SecondSeqGui::getSq()
{
    return sq;
}