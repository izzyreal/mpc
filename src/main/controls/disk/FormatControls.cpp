#include <controls/disk/FormatControls.hpp>

#include <lcdgui/LayeredScreen.hpp>

using namespace mpc::controls::disk;
using namespace std;

FormatControls::FormatControls(mpc::Mpc* mpc)
	: AbstractDiskControls(mpc)
{
}

void FormatControls::function(int i)
{
	init();
	switch (i) {
	case 0:
		ls.lock()->openScreen("load");
		break;
	case 1:
		ls.lock()->openScreen("save");
		break;
	}
}
