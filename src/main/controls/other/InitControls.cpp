#include <controls/other/InitControls.hpp>

#include <ui/UserDefaults.hpp>

using namespace mpc::controls::other;
using namespace std;

InitControls::InitControls() 
	: AbstractOtherControls()
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
		mpc::ui::UserDefaults::instance().initialize();
		ls.lock()->openScreen("sequencer");
		break;
	}
}
