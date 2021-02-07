#include "KeepOrRetryScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <controls/BaseSamplerControls.hpp>

#include <sampler/NoteParameters.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace std;

KeepOrRetryScreen::KeepOrRetryScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "keep-or-retry", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);
}

void KeepOrRetryScreen::open()
{
	init();
	displayNameForNewSound();
	displayAssignToNote();
	mpc.addObserver(this); // Subscribe to "padandnote" message
}

void KeepOrRetryScreen::close()
{
	mpc.deleteObserver(this);
}

void KeepOrRetryScreen::mainScreen() {
	
	sampler.lock()->deleteSound(sampler.lock()->getPreviewSound());
	baseControls->mainScreen();
}

void KeepOrRetryScreen::function(int i)
{
	init();
	
	switch (i)
    {
	case 1:
		sampler.lock()->deleteSound(sampler.lock()->getPreviewSound());
		openScreen("sample");
		break;
	case 3 :
		if (mpc.getControls().lock()->isF4Pressed())
		{
			return;
		}
		mpc.getControls().lock()->setF4Pressed(true);
		sampler.lock()->playPreviewSample(0, sampler.lock()->getPreviewSound().lock()->getLastFrameIndex(), 0, 2);
		break;
	case 4:
		auto index = sampler.lock()->getSoundCount() - 1;
		sampler.lock()->getLastNp(program.lock().get())->setSoundIndex(index);

		sampler.lock()->setSoundIndex(index);
		openScreen("sample");
		break;
	}
}

void KeepOrRetryScreen::turnWheel(int i)
{
    init();
	auto nameScreen = mpc.screens->get<NameScreen>("name");
	nameScreen->setName(sampler.lock()->getPreviewSound().lock()->getName());
	nameScreen->parameterName = param;
    openScreen("name");
}

void KeepOrRetryScreen::displayNameForNewSound()
{
	if (!sampler.lock()->getSound().lock())
		return;

    findField("name-for-new-sound").lock()->setText(sampler.lock()->getPreviewSound().lock()->getName());
}

void KeepOrRetryScreen::displayAssignToNote()
{
	auto note = to_string(sampler.lock()->getLastPad(program.lock().get())->getNote());
	auto pad = sampler.lock()->getPadName(sampler.lock()->getLastPad(program.lock().get())->getNumber());
	findField("assign-to-note").lock()->setText(note + "/" + pad);
}

void KeepOrRetryScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s.compare("padandnote") == 0)
		displayAssignToNote();
}
