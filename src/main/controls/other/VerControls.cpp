#include <controls/other/VerControls.hpp>

using namespace mpc::controls::other;
using namespace std;

VerControls::VerControls() 
	: AbstractOtherControls()
{
}

void VerControls::function(int i)
{
	init();
	switch (i) {
	case 0:
		ls.lock()->openScreen("others");
		break;
	case 1:
		ls.lock()->openScreen("init");
		break;
	}

}

VerControls::~VerControls() {
}
