#include <controls/other/dialog/NameControls.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <file/aps/ApsSaver.hpp>
#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Underline.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DirectoryGui.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <ui/vmpc/DirectToDiskRecorderGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::controls::other::dialog;
using namespace std;
using namespace moduru::lang;

NameControls::NameControls() 
	: AbstractOtherControls()
{
}

void NameControls::left()
{
	init();
	if (stoi(param) == 0) return;
	super::left();
	if (nameGui->isNameBeingEdited()) {
		auto field = ls.lock()->lookupField(ls.lock()->getFocus()).lock();
		field->setInverted(false);
		drawUnderline();
	}
}

void NameControls::right()
{
	init();
	if (stoi(param) == nameGui->getNameLimit() - 1) return;
	super::right();
	if (nameGui->isNameBeingEdited()) {
		auto field = ls.lock()->lookupField(ls.lock()->getFocus()).lock();
		field->setInverted(false);
		drawUnderline();
	}
}

void NameControls::turnWheel(int j)
{
    init();
	
	if (nameGui->isNameBeingEdited()) {
		for (int i = 0; i < 16; i++) {
			if (param.compare(to_string(i)) == 0) {
				nameGui->changeNameCharacter(i, j > 0);
				drawUnderline();
				break;
			}
		}
	}
	else {
		if (j > 0) {
			for (int i = 0; i < 16; i++) {
				if (param.compare(to_string(i)) == 0) {
					nameGui->changeNameCharacter(i, j > 0);
					nameGui->setNameBeingEdited(true);
					ls.lock()->getUnderline().lock()->Hide(false);
					initEditColors();
					drawUnderline();
					break;
				}
			}
		}
	}
}

void NameControls::function(int i)
{
    init();
    switch (i) {
    case 3:
        ls.lock()->openScreen(ls.lock()->getPreviousScreenName());
        resetNameGui();
        break;
	case 4:
		saveName();
		break;
    }

}

void NameControls::pressEnter() {
	saveName();
}

void NameControls::saveName() {
	auto uis = Mpc::instance().getUis().lock();
	auto lLs = ls.lock();
	auto lSequencer = sequencer.lock();
	auto ngParam = nameGui->getParameterName();
	auto prevScreen = lLs->getPreviousScreenName();
	auto soundGui = uis->getSoundGui();

	if (ngParam.compare("outputfolder") == 0) {
		uis->getD2DRecorderGui()->setOutputFolder(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("directtodiskrecorder");
	}
	else if (ngParam.compare("saveallfile") == 0) {
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("saveallfile");
		return;
	}
	else if (ngParam.compare("saveasound") == 0) {
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("saveasound");
		return;
	}
	else if (ngParam.compare("savingpgm") == 0) {
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("saveaprogram");
		return;
	}
	else if (ngParam.compare("savingaps") == 0) {
		string apsName = nameGui->getName();
		apsName.append(".APS");
		mpc::file::aps::ApsSaver(mpc::Util::getFileName(apsName));
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		return;
	}
	else if (ngParam.compare("savingmid") == 0) {
		lLs->openScreen("saveasequence");
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		return;
	}
	else if (ngParam.find("default") != string::npos) {
		if (prevScreen.compare("track") == 0) {
			lSequencer->setDefaultTrackName(nameGui->getName(), lSequencer->getActiveTrackIndex());
			nameGui->setNameBeingEdited(false);
			lLs->setLastFocus("name", "0");
			lLs->openScreen("sequencer");
			return;
		}
		else if (prevScreen.compare("sequence") == 0) {
			lSequencer->setDefaultSequenceName(nameGui->getName());
			nameGui->setNameBeingEdited(false);
			lLs->setLastFocus("name", "0");
			lLs->openScreen("sequencer");
			return;
		}
	}
	else if (ngParam.compare("programname") == 0) {
		program.lock()->setName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("program");
		return;
	}
	else if (ngParam.compare("createnewprogram") == 0) {
		uis->getSamplerWindowGui()->setNewName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("program");
		return;
	}
	else if (ngParam.compare("autochrom") == 0) {
		uis->getSamplerWindowGui()->setNewName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("autochromaticassignment");
		lLs->setPreviousScreenName(uis->getSamplerGui()->getPrevScreenName());
		return;
	}
	else if (ngParam.compare("rename") == 0) {
		bool success;
		auto ext = mpc::Util::splitName(directoryGui->getSelectedFile()->getName())[1];
		if (ext.length() > 0) ext = "." + ext;

		success = Mpc::instance().getDisk().lock()->renameSelectedFile(StrUtil::trim(StrUtil::toUpper(nameGui->getName())) + ext);
		if (!success) {
			lLs->createPopup("File name exists !!", 120);
			lLs->setPreviousScreenName("directory");
			return;
		}
		else {
			Mpc::instance().getDisk().lock()->flush();
			Mpc::instance().getDisk().lock()->initFiles();
			nameGui->setNameBeingEdited(false);
			lLs->setLastFocus("name", "0");
			lLs->openScreen("directory");
			return;
		}
	}
	else if (ngParam.compare("newfolder") == 0) {
		auto lDisk = Mpc::instance().getDisk().lock();
		bool success = lDisk->newFolder(StrUtil::toUpper(nameGui->getName()));
		if (success) {
			lDisk->flush();
			lDisk->initFiles();
			auto counter = 0;
			for (int i = 0; i < lDisk->getFileNames().size(); i++) {
				if (lDisk->getFileName(i).compare(StrUtil::toUpper(nameGui->getName())) == 0) {
					uis->getDiskGui()->setFileLoad(counter);
					if (counter > 4) {
						uis->getDirectoryGui()->setYOffset1(counter - 4);
					}
					else {
						uis->getDirectoryGui()->setYOffset1(0);
					}
					break;
				}
				counter++;
			}
			lLs->openScreen("directory");
			lLs->setPreviousScreenName("load");
			nameGui->setNameBeingEdited(false);
		}

		if (!success) {
			lLs->createPopup("Folder name exists !!", 120);
		}
	}

	if (prevScreen.compare("saveapsfile") == 0) {
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("saveapsfile");
	}
	else if (prevScreen.compare("keeporretry") == 0) {
		dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getPreviewSound().lock())->setName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("keeporretry");
	}
	else if (prevScreen.compare("track") == 0) {
		track.lock()->setName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("sequencer");
		return;
	}
	else if (prevScreen.compare("saveasequence") == 0) {
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("saveasequence");
	}
	else if (prevScreen.compare("sequence") == 0) {
		sequence.lock()->setName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("sequencer");
		return;
	}
	else if (prevScreen.compare("midioutput") == 0) {
		sequence.lock()->setDeviceName(swGui->getDeviceNumber() + 1, nameGui->getName().substr(0, 8));
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("sequencer");
		return;
	}
	else if (prevScreen.compare("editsound") == 0) {
		uis->getEditSoundGui()->setNewName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("editsound");
		return;
	}
	else if (prevScreen.compare("sound") == 0) {
		dynamic_pointer_cast<mpc::sampler::Sound>(Mpc::instance().getSampler().lock()->getSound(soundGui->getSoundIndex()).lock())->setName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("sound");
		return;
	}
	else if (prevScreen.compare("resample") == 0) {
		soundGui->setNewName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("resample");
		return;
	}
	else if (prevScreen.compare("stereotomono") == 0) {
		if (ngParam.compare("newlname") == 0) {
			soundGui->setNewLName(nameGui->getName());
		}
		else if (ngParam.compare("newrname") == 0) {
			soundGui->setNewRName(nameGui->getName());
		}
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("stereotomono");
		return;
	}
	else if (prevScreen.compare("copysound") == 0) {
		soundGui->setNewName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("copysound");
		return;
	}
}

void NameControls::drawUnderline()
{
	if (nameGui->isNameBeingEdited()) {
		string focus = ls.lock()->getFocus();
		if (focus.length() != 1 && focus.length() != 2) return;
		auto u = ls.lock()->getUnderline().lock();
		for (int i = 0; i < 16; i++) {
			if (i == stoi(focus)) {
				u->setState(i, true);
			}
			else {
				u->setState(i, false);
			}
		}
	}
}

void NameControls::initEditColors()
{
    for (int i = 0; i < 16; i++) {
		auto field = ls.lock()->lookupField(to_string(i)).lock();
		field->setOpaque(true);
		field->setInverted(false);
    }
	auto focus = ls.lock()->getFocus();
	ls.lock()->lookupField(focus).lock()->setInverted(false);
	ls.lock()->lookupField(focus).lock()->setOpaque(true);
}

void NameControls::resetNameGui()
{
	nameGui->setNameBeingEdited(false);
	ls.lock()->setLastFocus("name", "0");
}

NameControls::~NameControls() {
}
