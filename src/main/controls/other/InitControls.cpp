#include <controls/other/InitControls.hpp>

#include <StartUp.hpp>

using namespace mpc::controls::other;
using namespace std;

InitControls::InitControls(mpc::Mpc* mpc) 
	: AbstractOtherControls(mpc)
{
}

void InitControls::function(int i)
{
	init();
	switch (i) {
	case 0:
		ls.lock()->openScreen("others");
		break;
	case 2:
		ls.lock()->openScreen("ver");
		break;
	case 5:
		mpc::StartUp::initUserDefaults();
		ls.lock()->openScreen("sequencer");
		break;
	}
}
