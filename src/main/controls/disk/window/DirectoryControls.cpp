#include "DirectoryControls.hpp"

#include <Util.hpp>
#include <disk/MpcFile.hpp>
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
