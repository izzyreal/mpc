#include <controls/disk/window/LoadASoundControls.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <command/KeepSound.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

LoadASoundControls::LoadASoundControls(mpc::Mpc* mpc) 
	: AbstractDiskControls(mpc)
{
}

void LoadASoundControls::turnWheel(int i)
{
	init();
	if (param.compare("assigntonote") == 0) {
		auto nextNn = samplerGui->getNote() + i;
		auto nextPn = program.lock()->getPadNumberFromNote(nextNn);
		samplerGui->setPadAndNote(nextPn, nextNn);
	}
}

void LoadASoundControls::function(int i)
{
	init();
	auto controls = mpc->getControls().lock();
	shared_ptr<mpc::sampler::Sound> s;
	int start;
	int end;
	int loopTo;
	int overlapMode;
	auto lSampler = sampler.lock();
	switch (i) {
	case 2:
		
		if (controls->isF3Pressed()) {
			return;
		}

		controls->setF3Pressed(true);
		//lSampler->stopAllVoices();
		s = dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getPreviewSound().lock());
		start = s->getStart();
		end = s->getSampleData()->size();
		loopTo = -1;
		overlapMode = 1;
		if (s->isLoopEnabled()) {
			loopTo = s->getLoopTo();
			overlapMode = 2;
		}
		if (!s->isMono()) {
			end /= 2;
		}
		lSampler->playPreviewSample(start, end, loopTo, overlapMode);
		break;
	case 3:
		lSampler->finishBasicVoice(); // Here we make sure the sound is not being played, so it can be removed from memory.
		lSampler->deleteSound(dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getPreviewSound().lock()));
		ls.lock()->openScreen("load");
		break;
	case 4:
		auto command = mpc::command::KeepSound(mpc, dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getPreviewSound().lock()));
		command.execute();
		ls.lock()->openScreen("load");
		break;
	}
}

LoadASoundControls::~LoadASoundControls() {
}
