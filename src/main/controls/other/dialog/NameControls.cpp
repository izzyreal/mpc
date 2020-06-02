#include "NameControls.hpp"

#include <Mpc.hpp>

#include <lcdgui/Field.hpp>
#include <lcdgui/Underline.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/window/MidiOutputScreen.hpp>
#include <lcdgui/screens/window/MidiOutputScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/screens/window/AutoChromaticAssignmentScreen.hpp>
#include <lcdgui/screens/dialog/CopySoundScreen.hpp>
#include <lcdgui/screens/dialog/ResampleScreen.hpp>
#include <lcdgui/screens/dialog/StereoToMonoScreen.hpp>
#include <lcdgui/screens/dialog/CreateNewProgramScreen.hpp>

#include <Util.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <disk/ApsSaver.hpp>

#include <ui/NameGui.hpp>
#include <ui/vmpc/DirectToDiskRecorderGui.hpp>

#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::controls::other::dialog;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace std;
using namespace moduru::lang;

void NameControls::left()
{
	init();

	if (stoi(param) == 0)
	{
		return;
	}
	BaseControls::left();
	
	if (nameGui->isNameBeingEdited())
	{
		auto field = ls.lock()->lookupField(ls.lock()->getFocus()).lock();
		field->setInverted(false);
		drawUnderline();
	}
}

void NameControls::right()
{
	init();
	if (stoi(param) == nameGui->getNameLimit() - 1)
	{
		return;
	}
	
	BaseControls::right();
	
	if (nameGui->isNameBeingEdited())
	{
		auto field = ls.lock()->lookupField(ls.lock()->getFocus()).lock();
		field->setInverted(false);
		drawUnderline();
	}
}

void NameControls::turnWheel(int j)
{
    init();
	
	if (nameGui->isNameBeingEdited())
	{
		for (int i = 0; i < 16; i++)
		{
			if (param.compare(to_string(i)) == 0)
			{
				nameGui->changeNameCharacter(i, j > 0);
				drawUnderline();
				break;
			}
		}
	}
	else
	{
		if (j > 0)
		{
			for (int i = 0; i < 16; i++)
			{
				if (param.compare(to_string(i)) == 0)
				{
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
    switch (i)
	{
    case 3:
        ls.lock()->openScreen(ls.lock()->getPreviousScreenName());
        resetNameGui();
        break;
	case 4:
		saveName();
		break;
    }

}

void NameControls::pressEnter()
{
	saveName();
}

void NameControls::saveName()
{
	auto uis = Mpc::instance().getUis().lock();
	auto lLs = ls.lock();
	
	auto paramToRename = nameGui->getParameterName();
	auto prevScreen = lLs->getPreviousScreenName();

	if (paramToRename.compare("outputfolder") == 0)
	{
		uis->getD2DRecorderGui()->setOutputFolder(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("directtodiskrecorder");
	}
	else if (paramToRename.compare("save-all-file") == 0)
	{
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("save-all-file");
		return;
	}
	else if (paramToRename.compare("save-a-sound") == 0)
	{
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("save-a-sound");
		return;
	}
	else if (paramToRename.compare("savingpgm") == 0)
	{
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("saveaprogram");
		return;
	}
	else if (paramToRename.compare("savingaps") == 0)
	{
		string apsName = nameGui->getName();
		apsName.append(".APS");
		mpc::disk::ApsSaver apsSaver(mpc::Util::getFileName(apsName));
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		return;
	}
	else if (paramToRename.compare("savingmid") == 0)
	{
		lLs->openScreen("save-a-sequence");
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		return;
	}
	else if (paramToRename.find("default") != string::npos)
	{
		if (prevScreen.compare("track") == 0)
		{
			sequencer.lock()->setDefaultTrackName(nameGui->getName(), sequencer.lock()->getActiveTrackIndex());
			nameGui->setNameBeingEdited(false);
			lLs->setLastFocus("name", "0");
			lLs->openScreen("sequencer");
			return;
		}
		else if (prevScreen.compare("sequence") == 0)
		{
			sequencer.lock()->setDefaultSequenceName(nameGui->getName());
			nameGui->setNameBeingEdited(false);
			lLs->setLastFocus("name", "0");
			lLs->openScreen("sequencer");
			return;
		}
	}
	else if (paramToRename.compare("programname") == 0)
	{
		program.lock()->setName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("program");
		return;
	}
	else if (paramToRename.compare("create-new-program") == 0)
	{
		auto createNewProgramScreen = dynamic_pointer_cast<CreateNewProgramScreen>(Screens::getScreenComponent("create-new-program"));
		createNewProgramScreen->newName = nameGui->getName();
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("program");
		return;
	}
	else if (paramToRename.compare("autochrom") == 0)
	{
		auto autoChromaticAssignmentScreen = dynamic_pointer_cast<AutoChromaticAssignmentScreen>(Screens::getScreenComponent("auto-chromatic-assignment"));
		autoChromaticAssignmentScreen->newName = nameGui->getName();
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("auto-chromatic-assignment");
		lLs->setPreviousScreenName(mpc.getPreviousSamplerScreenName());
		return;
	}
	else if (paramToRename.compare("rename") == 0)
	{
		auto directoryScreen = dynamic_pointer_cast<DirectoryScreen>(Screens::getScreenComponent("directory"));
		auto ext = mpc::Util::splitName(directoryScreen->getSelectedFile()->getName())[1];
		
		if (ext.length() > 0)
		{
			ext = "." + ext;
		}

		bool success = Mpc::instance().getDisk().lock()->renameSelectedFile(StrUtil::trim(StrUtil::toUpper(nameGui->getName())) + ext);
		
		if (!success)
		{
			lLs->createPopup("File name exists !!", 120);
			lLs->setPreviousScreenName("directory");
			return;
		}
		else
		{
			Mpc::instance().getDisk().lock()->flush();
			Mpc::instance().getDisk().lock()->initFiles();
			nameGui->setNameBeingEdited(false);
			lLs->setLastFocus("name", "0");
			lLs->openScreen("directory");
			return;
		}
	}
	else if (paramToRename.compare("newfolder") == 0)
 {
		auto lDisk = Mpc::instance().getDisk().lock();
		bool success = lDisk->newFolder(StrUtil::toUpper(nameGui->getName()));

		if (success)
		{
			lDisk->flush();
			lDisk->initFiles();
			auto counter = 0;

			for (int i = 0; i < lDisk->getFileNames().size(); i++)
			{
				if (lDisk->getFileName(i).compare(StrUtil::toUpper(nameGui->getName())) == 0)
				{
					auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));
					loadScreen->setFileLoad(counter);

					auto directoryScreen = dynamic_pointer_cast<DirectoryScreen>(Screens::getScreenComponent("directory"));

					if (counter > 4)
					{
						directoryScreen->yOffset1 = counter - 4;
					}
					else
					{
						directoryScreen->yOffset1 = 0;
					}
					break;
				}
				counter++;
			}

			lLs->openScreen("directory");
			lLs->setPreviousScreenName("load");
			nameGui->setNameBeingEdited(false);
		}

		if (!success)
		{
			lLs->createPopup("Folder name exists !!", 120);
		}
	}

	if (prevScreen.compare("save-aps-file") == 0)
	{
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("save-aps-file");
	}
	else if (prevScreen.compare("keep-or-retry") == 0)
	{
		dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getPreviewSound().lock())->setName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("keep-or-retry");
	}
	else if (prevScreen.compare("track") == 0
		) {
		track.lock()->setName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("sequencer");
		return;
	}
	else if (prevScreen.compare("save-a-sequence") == 0)
	{
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("save-a-sequence");
	}
	else if (prevScreen.compare("sequence") == 0)
	{
		sequence.lock()->setName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("sequencer");
		return;
	}
	else if (prevScreen.compare("midioutput") == 0)
	{
		auto midiOutputScreen = dynamic_pointer_cast<MidiOutputScreen>(Screens::getScreenComponent("midioutput"));
		sequence.lock()->setDeviceName(midiOutputScreen->getDeviceNumber() + 1, nameGui->getName().substr(0, 8));
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("sequencer");
		return;
	}
	else if (prevScreen.compare("edit-sound") == 0)
	{
		auto editSoundScreen = dynamic_pointer_cast<EditSoundScreen>(Screens::getScreenComponent("edit-sound"));
		editSoundScreen->setNewName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("edit-sound");
		return;
	}
	else if (prevScreen.compare("sound") == 0)
	{
		sampler.lock()->getSound().lock()->setName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("sound");
		return;
	}
	else if (prevScreen.compare("resample") == 0)
	{
		auto resampleScreen = dynamic_pointer_cast<ResampleScreen>(Screens::getScreenComponent("resample"));
		resampleScreen->setNewName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("resample");
		return;
	}
	else if (prevScreen.compare("stereo-to-mono") == 0)
	{
		auto stereoToMonoScreen = dynamic_pointer_cast<StereoToMonoScreen>(Screens::getScreenComponent("stereo-to-mono"));
		if (paramToRename.compare("newlname") == 0)
		{
			stereoToMonoScreen->setNewLName(nameGui->getName());
		}
		else if (paramToRename.compare("newrname") == 0)
		{
			stereoToMonoScreen->setNewRName(nameGui->getName());
		}
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("stereo-to-mono");
		return;
	}
	else if (prevScreen.compare("copy-sound") == 0)
	{
		auto copySoundScreen = dynamic_pointer_cast<CopySoundScreen>(Screens::getScreenComponent("copy-sound"));
		copySoundScreen->setNewName(nameGui->getName());
		nameGui->setNameBeingEdited(false);
		lLs->setLastFocus("name", "0");
		lLs->openScreen("copy-sound");
		return;
	}
}

void NameControls::drawUnderline()
{
	if (nameGui->isNameBeingEdited())
	{
		string focus = ls.lock()->getFocus();
	
		if (focus.length() != 1 && focus.length() != 2)
		{
			return;
		}
		
		auto u = ls.lock()->getUnderline().lock();
		
		for (int i = 0; i < 16; i++)
		{
			if (i == stoi(focus))
			{
				u->setState(i, true);
			}
			else
			{
				u->setState(i, false);
			}
		}
	}
}

void NameControls::initEditColors()
{
    for (int i = 0; i < 16; i++)
	{
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
