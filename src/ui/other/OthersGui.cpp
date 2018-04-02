#include <ui/other/OthersGui.hpp>

#include <Util.hpp>
#include <lcdgui/Field.hpp>

using namespace mpc::ui::other;
using namespace std;

OthersGui::OthersGui(mpc::Mpc* mpc)
{
	this->mpc = mpc;
}

void OthersGui::setTapAveraging(int i)
{
	if (i < 0 || i > 8) return;
	tapAveraging = i;
	setChanged();
	notifyObservers(string("tapaveraging"));
}

int OthersGui::getTapAveraging()
{
	return tapAveraging;
}

void OthersGui::setContrast(int i)
{
	if (i < 0 || i > 50) return;
	contrast = i;
	/*
	auto f = (float) (1.0f - (contrast / 50.0));
	auto red = orig->R + ((255 - orig->R) * f);
	auto green = orig->G + ((255 - orig->G) * f);
	auto blue = orig->B + ((255 - orig->B) * f);
	auto lMainFrame = lGui->getMainFrame().lock();
	//mpc::maingui::StartUp::lcdOn() = new ::java::awt::Color(red, green, blue);
	//lGui->getMainFrame()->repaint();
	auto focus = lMainFrame->getFocus(0);
	auto tf = lMainFrame->lookupField(focus);
	//tf->setBackground(mpc::maingui::StartUp::lcdOn());
	auto label = lMainFrame->lookupLabel(focus);
	//label->setForeground(mpc::maingui::StartUp::lcdOn());
	//tf->repaint();
	//label->repaint();
	*/
	setChanged();
	notifyObservers(string("contrast"));
}

int OthersGui::getContrast()
{
    return contrast;
}

OthersGui::~OthersGui() {
}
