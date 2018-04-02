#include <controls/vmpc/AudioMidiDisabledControls.hpp>

using namespace mpc::controls::vmpc;
using namespace std;

AudioMidiDisabledControls::AudioMidiDisabledControls(mpc::Mpc* mpc)
	: AbstractVmpcControls(mpc)
{
}

void AudioMidiDisabledControls::function(int i)
{
	init();
	switch (i) {
	case 3:
		ls.lock()->openScreen("sequencer");
		break;
	}
}
