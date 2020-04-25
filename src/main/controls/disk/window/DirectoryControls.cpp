#include "DirectoryControls.hpp"

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundPlayer.hpp>

#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <Util.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <disk/SoundLoader.hpp>

#include <ui/NameGui.hpp>
#include <ui/disk/window/DirectoryGui.hpp>

#include <lcdgui/Field.hpp>

#include <file/File.hpp>

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

		auto file = directoryGui->getSelectedFile();

		if (!file->isDirectory()) {
			
			bool started = mpc->getAudioMidiServices().lock()->getSoundPlayer().lock()->start(file->getFile().lock()->getPath());
			
			auto name = file->getFsNode().lock()->getNameWithoutExtension();

			if (started) {
				mpc->getLayeredScreen().lock()->createPopup("Playing " + name, 45);
			}
			else {
				mpc->getLayeredScreen().lock()->createPopup("Can't play " + name, 35);
			}
		}

		break;
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
		directoryGui->down();
	}
	else {
		directoryGui->up();
	}
}
