#include "DirectoryControls.hpp"

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <Util.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <disk/SoundLoader.hpp>

#include <ui/NameGui.hpp>
#include <ui/disk/window/DirectoryGui.hpp>

#include <lcdgui/Field.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

DirectoryControls::DirectoryControls(mpc::Mpc* mpc)
	: AbstractDiskControls(mpc)
{
}

void DirectoryControls::function(int f)
{
	super::function(f);
	string fileNameNoExt;
	switch (f) {
	case 1:
		if (directoryGui->getSelectedFile() == nullptr) return;

		if (directoryGui->getSelectedFile()->isDirectory()) {
			ls.lock()->openScreen("deletefolder");
		}
		else {
			ls.lock()->openScreen("deletefile");
		}
		break;
	case 2:
		if (directoryGui->getSelectedFile() == nullptr) return;

		fileNameNoExt = mpc::Util::splitName(directoryGui->getSelectedFile()->getName())[0];
		nameGui->setName(fileNameNoExt);
		if (directoryGui->getSelectedFile()->isDirectory()) {
			nameGui->setNameLimit(8);
		}
		nameGui->setParameterName("rename");
		ls.lock()->openScreen("name");
		break;
	case 4:
		if (directoryGui->getXPos() == 0) return;

		nameGui->setName("NEWFOLDR");
		nameGui->setNameLimit(8);
		nameGui->setParameterName("newfolder");
		ls.lock()->openScreen("name");
		break;
	case 5:
	{
		auto controls = mpc->getControls().lock();

		if (controls->isF6Pressed()) {
			return;
		}

		controls->setF6Pressed(true);

		auto soundLoader = mpc::disk::SoundLoader(mpc, vector<weak_ptr<mpc::sampler::Sound>>(), false);

		soundLoader.setPreview(true);
		soundLoader.setPartOfProgram(true); // hack to disable popups

		try {
			soundLoader.loadSound(directoryGui->getSelectedFile()) == -1;
		}
		catch (const invalid_argument& exception) {
			mpc->getDisk().lock()->setBusy(false);
			return;
		}

		mpc->getDisk().lock()->setBusy(false);

		auto lSampler = sampler.lock();
		auto s = dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getPreviewSound().lock());
		auto start = s->getStart();
		auto end = s->getSampleData()->size();
		auto loopTo = -1;
		auto overlapMode = 1;

		if (s->isLoopEnabled()) {
			loopTo = s->getLoopTo();
			overlapMode = 2;
		}

		if (!s->isMono()) {
			end /= 2;
		}

		lSampler->playPreviewSample(start, end, loopTo, overlapMode);
	}
	}
}

void DirectoryControls::left()
{
    init();
    directoryGui->left();
}

void DirectoryControls::right()
{
	init();
	directoryGui->right();
}

void DirectoryControls::up()
{
    init();
    directoryGui->up();
}

void DirectoryControls::down()
{
    init();
    directoryGui->down();
}

void DirectoryControls::turnWheel(int i)
{
	init();
	if (i > 0) {
		directoryGui->up();
	}
	else {
		directoryGui->down();
	}
}
