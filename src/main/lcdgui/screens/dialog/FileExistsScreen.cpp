#include "FileExistsScreen.hpp"

#include <Util.hpp>

#include "nvram/MidiControlPersistence.hpp"

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#include <file/all/AllParser.hpp>

#include <lcdgui/screens/window/SaveASoundScreen.hpp>
#include "sampler/Sound.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;
using namespace mpc::nvram;

using namespace moduru::lang;

FileExistsScreen::FileExistsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "file-exists", layerIndex)
{
}

void FileExistsScreen::mainScreen()
{
	if (loadASoundCandidate && existingSound)
	{
		sampler->deleteSound(sampler->getPreviewSound());
		loadASoundCandidate = {};
		existingSound = {};
		actionAfterAddingSound = [](bool){};
	}
	ScreenComponent::mainScreen();
}

void FileExistsScreen::setLoadASoundCandidateAndExistingSound(
		std::shared_ptr<Sound> candidate,
		std::shared_ptr<Sound> existing)
{
	loadASoundCandidate = candidate;
	existingSound = existing;
}

void FileExistsScreen::setActionAfterAddingSound(std::function<void(bool)> action)
{
	actionAfterAddingSound = action;
}

void FileExistsScreen::function(int i)
{
	switch (i)
	{
	case 2:
	    replaceAction();
        break;
	case 3:
        openScreen(cancelScreen);
		break;
	case 4:
        initializeNameScreen();
        openScreen("name");
	}
}

void FileExistsScreen::initialize(std::function<void()> replaceActionToUse, std::function<void()> initializeNameScreenToUse,
                                  std::string replaceOrCancelScreenToUse)
{
    replaceAction = replaceActionToUse;
    initializeNameScreen = initializeNameScreenToUse;
    cancelScreen = replaceOrCancelScreenToUse;
}
