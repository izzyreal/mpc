#include <ui/disk/window/SaveAllFileObserver.hpp>

#include <Mpc.hpp>
#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/disk/DiskGui.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::disk::window;
using namespace std;

SaveAllFileObserver::SaveAllFileObserver(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	auto ls = mpc->getLayeredScreen().lock();
	fileField = ls->lookupField("file");
	fileLabel = ls->lookupLabel("file1");
	diskGui = mpc->getUis().lock()->getDiskGui();
	displayFile();
}

void SaveAllFileObserver::displayFile()
{
	fileField.lock()->setText(mpc->getUis().lock()->getNameGui()->getName().substr(0, 1));
	fileLabel.lock()->setText(moduru::lang::StrUtil::padRight(mpc->getUis().lock()->getNameGui()->getName().substr(1), " ", 15) + ".ALL");
}

void SaveAllFileObserver::update(moduru::observer::Observable* o, boost::any a)
{
}

SaveAllFileObserver::~SaveAllFileObserver() {
}
