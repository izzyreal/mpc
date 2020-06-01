#include <ui/disk/DiskObserver.hpp>

#include <Mpc.hpp>
#include <Paths.hpp>
#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DiskWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Track.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lang/StrUtil.hpp>
#include <file/FileUtil.hpp>
#include <file/File.hpp>

using namespace mpc::ui::disk;
using namespace moduru::lang;
using namespace std;

DiskObserver::DiskObserver()
{
	auto uis = Mpc::instance().getUis().lock();

	diskWindowGui = uis->getDiskWindowGui();
	diskWindowGui->addObserver(this);
	Mpc::instance().addObserver(this);
	disk = Mpc::instance().getDisk();
	sampler = Mpc::instance().getSampler();
	sequencer = Mpc::instance().getSequencer();
	auto lSequencer = sequencer.lock();
	lSequencer->addObserver(this);
	diskGui = Mpc::instance().getUis().lock()->getDiskGui();
	diskGui->addObserver(this);
	auto lSampler = sampler.lock();
	int activeTrack = lSequencer->getActiveTrackIndex();
	int drum = lSequencer->getActiveSequence().lock()->getTrack(activeTrack).lock()->getBusNumber() - 1;
	int candidate = drum;

	if (candidate < 0) {
		candidate = 0;
	}

	mpcSoundPlayerChannel = lSampler->getDrum(candidate);
	mpcSoundPlayerChannel->addObserver(this);
	program = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock());

	csn = Mpc::instance().getLayeredScreen().lock()->getCurrentScreenName();

	auto lDisk = disk.lock();
	
	if (lDisk) {
		lDisk->addObserver(this);
	}
	
	auto ls = Mpc::instance().getLayeredScreen().lock();
	
	if (csn.compare("cantfindfile") == 0) {
		fileField = ls->lookupField("file");
		fileField.lock()->setText(*diskGui->getCannotFindFileName());
	}
}

void DiskObserver::update(moduru::observer::Observable* o, nonstd::any a)
{
	string param = "";
	try {
		param = nonstd::any_cast<string>(a);
	}
	catch (std::exception& e) {
		return;
	}

	if (param.compare("savesequenceas") == 0) {
		displaySaveAs();
	}
	else if (param.compare("pgm") == 0) {
		displayFile();
	}
	else if (param.compare("directory") == 0) {
		displayDirectory();
	}
	else if (param.compare("removepopup") == 0) {
		Mpc::instance().getLayeredScreen().lock()->removePopup();
	}
	else if (param.compare("padandnote") == 0 || param.compare("note") == 0) {
		if (csn.compare("load-a-sound") == 0)
			displayAssignToNote();
	}
}

void DiskObserver::displayDirectory()
{
	auto lDisk = disk.lock();
	directoryField.lock()->setText(lDisk->getDirectoryName());
}

DiskObserver::~DiskObserver() {
	auto lDisk = disk.lock();
	lDisk->deleteObserver(this);
	auto lSequencer = sequencer.lock();
	lSequencer->deleteObserver(this);
	diskWindowGui->deleteObserver(this);
	Mpc::instance().deleteObserver(this);
	diskGui->deleteObserver(this);
	mpcSoundPlayerChannel->deleteObserver(this);
}
