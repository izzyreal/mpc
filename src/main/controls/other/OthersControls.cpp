#include <controls/other/OthersControls.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/other/OthersGui.hpp>

using namespace mpc::controls::other;
using namespace std;

OthersControls::OthersControls(mpc::Mpc* mpc) 
	: BaseControls(mpc)
{
	othersGui = mpc->getUis().lock()->getOthersGui();
}

void OthersControls::function(int i)
{
	init();
	switch (i) {
	case 1:
		ls.lock()->openScreen("init");
		break;
	case 2:
		ls.lock()->openScreen("ver");
		break;
	}
}

void OthersControls::turnWheel(int i)
{
    init();
	auto controls = mpc->getControls().lock();
	if (param.compare("tapaveraging") == 0) {
		othersGui->setTapAveraging(othersGui->getTapAveraging() + i);
	}
	//if (mpc::controls::KbMouseController::altIsPressed()) {
		//othersGui->setContrast(othersGui->getContrast() + i);
	//}
}


OthersControls::~OthersControls() {
}
