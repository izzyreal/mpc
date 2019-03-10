#include <ui/other/OthersObserver.hpp>

#include <Mpc.hpp>
#include <lcdgui/Field.hpp>
#include <ui/other/OthersGui.hpp>

using namespace mpc::ui::other;
using namespace std;

OthersObserver::OthersObserver(mpc::Mpc* mpc) 
{
	othersGui = mpc->getUis().lock()->getOthersGui();
	othersGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	tapAveragingField = ls->lookupField("tapaveraging");
	displayTapAveraging();
}

void OthersObserver::displayTapAveraging()
{
	tapAveragingField.lock()->setText(to_string(othersGui->getTapAveraging()));
}

void OthersObserver::update(moduru::observer::Observable* o, std::any a)
{
	string param = std::any_cast<string>(a);
    if (param.compare("tapaveraging") == 0) {
        displayTapAveraging();
    }

}

OthersObserver::~OthersObserver() {
	othersGui->deleteObserver(this);
}
