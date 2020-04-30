#include <controls/vmpc/RecordingFinishedControls.hpp>

using namespace mpc::controls::vmpc;
using namespace std;

RecordingFinishedControls::RecordingFinishedControls()
	: AbstractVmpcControls()
{
}

void RecordingFinishedControls::function(int i)
{
    init();
	switch (i) {
    case 3:
        ls.lock()->openScreen("sequencer");
        break;
    }
}
