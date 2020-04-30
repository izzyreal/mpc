#include <ui/other/OthersObserver.hpp>

#include <Mpc.hpp>
#include <lcdgui/Field.hpp>
#include <ui/other/OthersGui.hpp>

using namespace mpc::ui::other;
using namespace std;

OthersObserver::OthersObserver() 
{
	othersGui = Mpc::instance().getUis().lock()->getOthersGui();
	othersGui->addObserver(this);
	auto ls = Mpc::instance().getLayeredScreen().lock();
	tapAveragingField = ls->lookupField("tapaveraging");
	displayTapAveraging();
}

void OthersObserver::displayTapAveraging()
{
	tapAveragingField.lock()->setText(to_string(othersGui->getTapAveraging()));
}

void OthersObserver::update(moduru::observer::Observable* o, nonstd::any a)
{
	string param = nonstd::any_cast<string>(a);
    if (param.compare("tapaveraging") == 0) {
        displayTapAveraging();
    }

}

OthersObserver::~OthersObserver() {
	othersGui->deleteObserver(this);
}
