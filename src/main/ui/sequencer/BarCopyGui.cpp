#include <ui/sequencer/BarCopyGui.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

BarCopyGui::BarCopyGui() 
{
}

int BarCopyGui::getLastBar()
{
    return lastBar;
}

void BarCopyGui::setLastBar(int i, int max)
{
	if (i < 0 || i > max) return;

	lastBar = i;
	if (lastBar < firstBar) {
		setFirstBar(lastBar, max);
	}
	setChanged();
	notifyObservers(string("lastbar"));
}

int BarCopyGui::getFromSq()
{
    return fromSq;
}

void BarCopyGui::setFromSq(int i)
{
    if(i < 0 || i > 98) return;

    fromSq = i;
    setChanged();
    notifyObservers(string("fromsq"));
}

int BarCopyGui::getToSq()
{
    return toSq;
}

void BarCopyGui::setToSq(int i)
{
    if(i < 0 || i > 98)
        return;

    toSq = i;
    setChanged();
    notifyObservers(string("tosq"));
}

int BarCopyGui::getFirstBar()
{
    return firstBar;
}

void BarCopyGui::setFirstBar(int i, int max)
{
    if(i < 0 || i > max)
        return;

    firstBar = i;
    if(firstBar > lastBar)
        setLastBar(firstBar, max);

    setChanged();
    notifyObservers(string("firstbar"));
}

int BarCopyGui::getAfterBar()
{
    return afterBar;
}

void BarCopyGui::setAfterBar(int i, int max)
{
    if(i < 0 || i > max + 1)
        return;

    afterBar = i;
    setChanged();
    notifyObservers(string("afterbar"));
}

int BarCopyGui::getCopies()
{
    return copies;
}

void BarCopyGui::setCopies(int i)
{
    if(i < 1 || i > 999)
        return;

    copies = i;
    setChanged();
    notifyObservers(string("copies"));
}