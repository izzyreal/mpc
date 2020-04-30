#include <ui/NameObserver.hpp>

#include <lcdgui/Underline.hpp>

#include <string>

using namespace mpc::ui;
using namespace mpc::lcdgui;
using namespace std;

NameObserver::NameObserver()
{
	
	nameGui = Mpc::instance().getUis().lock()->getNameGui();
	auto ls = Mpc::instance().getLayeredScreen().lock();
	a0Field = ls->lookupField("0");
	a1Field = ls->lookupField("1");
	a2Field = ls->lookupField("2");
	a3Field = ls->lookupField("3");
	a4Field = ls->lookupField("4");
	a5Field = ls->lookupField("5");
	a6Field = ls->lookupField("6");
	a7Field = ls->lookupField("7");
	a8Field = ls->lookupField("8");
	a9Field = ls->lookupField("9");
	a10Field = ls->lookupField("10");
	a11Field = ls->lookupField("11");
	a12Field = ls->lookupField("12");
	a13Field = ls->lookupField("13");
	a14Field = ls->lookupField("14");
	a15Field = ls->lookupField("15");

	nameGui->deleteObservers();
	nameGui->addObserver(this);
	underline = ls->getUnderline();
	underline.lock()->Hide(true);
	for (int i = 0; i < 16; i++) {
		underline.lock()->setState(i, false);
	}
	displayName();
}

void NameObserver::displayName()
{
	a0Field.lock()->setText(nameGui->getName().substr(0, 1));
	a1Field.lock()->setText(nameGui->getName().substr(1, 1));
	a2Field.lock()->setText(nameGui->getName().substr(2, 1));
	a3Field.lock()->setText(nameGui->getName().substr(3, 1));
	a4Field.lock()->setText(nameGui->getName().substr(4, 1));
	a5Field.lock()->setText(nameGui->getName().substr(5, 1));
	a6Field.lock()->setText(nameGui->getName().substr(6, 1));
	a7Field.lock()->setText(nameGui->getName().substr(7, 1));
	if (nameGui->getNameLimit() > 8) {
		a8Field.lock()->Hide(false);
		a9Field.lock()->Hide(false);
		a10Field.lock()->Hide(false);
		a11Field.lock()->Hide(false);
		a12Field.lock()->Hide(false);
		a13Field.lock()->Hide(false);
		a14Field.lock()->Hide(false);
		a15Field.lock()->Hide(false);
		a8Field.lock()->setText(nameGui->getName().substr(8, 1));
		a9Field.lock()->setText(nameGui->getName().substr(9, 1));
		a10Field.lock()->setText(nameGui->getName().substr(10, 1));
		a11Field.lock()->setText(nameGui->getName().substr(11, 1));
		a12Field.lock()->setText(nameGui->getName().substr(12, 1));
		a13Field.lock()->setText(nameGui->getName().substr(13, 1));
		a14Field.lock()->setText(nameGui->getName().substr(14, 1));
		a15Field.lock()->setText(nameGui->getName().substr(15, 1));
	}
	else {
		a8Field.lock()->Hide(true);
		a9Field.lock()->Hide(true);
		a10Field.lock()->Hide(true);
		a11Field.lock()->Hide(true);
		a12Field.lock()->Hide(true);
		a13Field.lock()->Hide(true);
		a14Field.lock()->Hide(true);
		a15Field.lock()->Hide(true);
	}
}

void NameObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	auto ls = Mpc::instance().getLayeredScreen().lock();
	string s = nonstd::any_cast<string>(arg);
	string str;
	shared_ptr<Field> tf;
	if (s.compare("name") == 0) {
		str = ls->getFocus();
		tf = ls->lookupField(str).lock();
		tf->setText(nameGui->getName().substr(stoi(str), 1));
	}
}

NameObserver::~NameObserver() {
	nameGui->deleteObserver(this);
	if (underline.lock()) underline.lock()->Hide(true);
}
