#include <controls/misc/SecondSeqControls.hpp>

#include <ui/misc/SecondSeqGui.hpp>

using namespace mpc::controls::misc;
using namespace std;

SecondSeqControls::SecondSeqControls(mpc::Mpc* mpc)
	: AbstractMiscControls(mpc)
{
}

void SecondSeqControls::function(int i)
{
    init();
	switch (i) {
    case 0:
        ls.lock()->openScreen("punch");
        break;
    case 1:
		ls.lock()->openScreen("trans");
        break;
    case 5:
        break;
    }
}

void SecondSeqControls::turnWheel(int i)
{
	init();
	if (param.compare("sq") == 0) {
		secondSeqGui->setSq(secondSeqGui->getSq() + i);
	}
}

SecondSeqControls::~SecondSeqControls() {
}
