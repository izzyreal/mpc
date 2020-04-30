#include <ui/disk/window/SaveAllFileObserver.hpp>

#include <Mpc.hpp>
#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/disk/DiskGui.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::disk::window;
using namespace std;

SaveAllFileObserver::SaveAllFileObserver()
{
	
	auto ls = Mpc::instance().getLayeredScreen().lock();
	fileField = ls->lookupField("file");
	fileLabel = ls->lookupLabel("file1");
	diskGui = Mpc::instance().getUis().lock()->getDiskGui();
	displayFile();
}

void SaveAllFileObserver::displayFile()
{
	fileField.lock()->setText(Mpc::instance().getUis().lock()->getNameGui()->getName().substr(0, 1));
	fileLabel.lock()->setText(moduru::lang::StrUtil::padRight(Mpc::instance().getUis().lock()->getNameGui()->getName().substr(1), " ", 15) + ".ALL");
}

void SaveAllFileObserver::update(moduru::observer::Observable* o, nonstd::any a)
{
}

SaveAllFileObserver::~SaveAllFileObserver() {
}
