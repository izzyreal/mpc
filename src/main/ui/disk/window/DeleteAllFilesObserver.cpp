#include <ui/disk/window/DeleteAllFilesObserver.hpp>

#include <Mpc.hpp>
#include <lcdgui/Field.hpp>
#include <ui/disk/window/DiskWindowGui.hpp>

using namespace mpc::ui::disk::window;
using namespace std;

DeleteAllFilesObserver::DeleteAllFilesObserver(mpc::Mpc* mpc)
{
	views = vector<string>{ "All Files", ".SND", ".PGM", ".APS", ".MID", ".ALL", ".WAV", ".SEQ", ".SET" };
	diskWindowGui = mpc->getUis().lock()->getDiskWindowGui();
	csn = mpc->getLayeredScreen().lock()->getCurrentScreenName();
	diskWindowGui->addObserver(this);
	deleteField = mpc->getLayeredScreen().lock()->lookupField("delete");
	displayDelete();
}

void DeleteAllFilesObserver::displayDelete()
{
	deleteField.lock()->setText(views[diskWindowGui->getDelete()]);
}

void DeleteAllFilesObserver::update(moduru::observer::Observable* o, nonstd::any a)
{
	string param = nonstd::any_cast<string>(a);
	if (param.compare("delete") == 0) {
		displayDelete();
	}
}

DeleteAllFilesObserver::~DeleteAllFilesObserver() {
	diskWindowGui->deleteObserver(this);
}
