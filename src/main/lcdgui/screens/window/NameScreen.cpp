#include "NameScreen.hpp"

#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>
#include <lcdgui/screens/dialog/CopySoundScreen.hpp>
#include <lcdgui/screens/dialog/ResampleScreen.hpp>
#include <lcdgui/screens/dialog/StereoToMonoScreen.hpp>
#include <lcdgui/screens/dialog/MonoToStereoScreen.hpp>
#include <lcdgui/screens/dialog/CreateNewProgramScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <lcdgui/Underline.hpp>

#include <Util.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace std;
using namespace moduru::lang;

NameScreen::NameScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "name", layerIndex)
{
	addChild(make_shared<Underline>());
}

void NameScreen::setRenamerAndScreenToReturnTo(const std::function<void(string&)>& renamer, const string& screen)
{
    this->renamer = renamer;
    screenToReturnTo = screen;
}

weak_ptr<Underline> NameScreen::findUnderline()
{
	return dynamic_pointer_cast<Underline>(findChild("underline").lock());
}

void NameScreen::open()
{
	for (int i = 0; i < 16; i++)
		findUnderline().lock()->setState(i, false);

	displayName();
}

void NameScreen::close()
{
    ls.lock()->setLastFocus("name", "0");
    editing = false;
    parameterName = "";
}

void NameScreen::left()
{
	init();

	if (stoi(param) == 0)
		return;

	ScreenComponent::left();
	
	if (editing)
	{
		auto field = findField(ls.lock()->getFocus()).lock();
		field->setInverted(false);
		drawUnderline();
	}
}

void NameScreen::right()
{
	init();

	if (stoi(param) == nameLimit - 1)
		return;
	
	ScreenComponent::right();
	
	if (editing)
	{
		auto field = findField(ls.lock()->getFocus()).lock();
		field->setInverted(false);
		drawUnderline();
	}
}

void NameScreen::turnWheel(int j)
{
	init();

	if (editing)
	{
		for (int i = 0; i < 16; i++)
		{
			if (param.compare(to_string(i)) == 0)
			{
				changeNameCharacter(i, j > 0);
				drawUnderline();
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < 16; i++)
		{
			if (param.compare(to_string(i)) == 0)
			{
				changeNameCharacter(i, j > 0);
				editing = true;
				initEditColors();
				drawUnderline();
				break;
			}
		}
	}
}

void NameScreen::function(int i)
{
    init();

    switch (i)
	{
    case 3:
	{
		name = originalName;

		vector<string> screens{ "save-a-program", "save-a-sequence", "save-aps-file", "save-all-file", "save-a-sound" };

		if (find(begin(screens), end(screens), parameterName) != end(screens))
			openScreen(parameterName);
		else
			openScreen(ls.lock()->getPreviousScreenName());
		break;
	}
	case 4:
		saveName();
		break;
    }
}

void NameScreen::pressEnter()
{
	saveName();
}

void NameScreen::saveName()
{	
	auto prevScreen = ls.lock()->getPreviousScreenName();

    if (parameterName.compare("save-all-file") == 0)
	{
		openScreen("save-all-file");
		return;
	}
	else if (parameterName.compare("save-a-sound") == 0)
	{
		openScreen("save-a-sound");
		return;
	}
	else if (parameterName.compare("save-a-program") == 0)
	{
		openScreen("save-a-program");
		return;
	}
	else if (parameterName.compare("save-a-sequence") == 0)
	{
		openScreen("save-a-sequence");
		return;
	}

	if (prevScreen.compare("save-aps-file") == 0)
	{
		openScreen(prevScreen);
	}
	else if (prevScreen.compare("keep-or-retry") == 0)
	{
		sampler.lock()->getPreviewSound().lock()->setName(getNameWithoutSpaces());
		openScreen(prevScreen);
	}
    else if (prevScreen.compare("save-a-sequence") == 0)
	{
		openScreen(prevScreen);
	}
	else if (prevScreen.compare("edit-sound") == 0)
	{
		auto editSoundScreen = mpc.screens->get<EditSoundScreen>("edit-sound");
		editSoundScreen->setNewName(getNameWithoutSpaces());
		openScreen(prevScreen);
	}
	else if (prevScreen.compare("resample") == 0)
	{
		if (sampler.lock()->isSoundNameOccupied(getNameWithoutSpaces()))
			return;

		auto resampleScreen = mpc.screens->get<ResampleScreen>("resample");
		resampleScreen->setNewName(getNameWithoutSpaces());
		openScreen(prevScreen);
	}
	else if (prevScreen.compare("stereo-to-mono") == 0)
	{
		auto stereoToMonoScreen = mpc.screens->get<StereoToMonoScreen>("stereo-to-mono");

		if (parameterName.compare("newlname") == 0)
			stereoToMonoScreen->setNewLName(getNameWithoutSpaces());

		else if (parameterName.compare("newrname") == 0)
			stereoToMonoScreen->setNewRName(getNameWithoutSpaces());

		openScreen(prevScreen);
	}
	else if (prevScreen.compare("mono-to-stereo") == 0)
	{
		auto monoToStereoScreen = mpc.screens->get<MonoToStereoScreen>("mono-to-stereo");

		monoToStereoScreen->newStName = getNameWithoutSpaces();

		openScreen(prevScreen);
	}
	else if (prevScreen.compare("copy-sound") == 0)
	{
		auto copySoundScreen = mpc.screens->get<CopySoundScreen>("copy-sound");
		copySoundScreen->setNewName(getNameWithoutSpaces());
		openScreen(prevScreen);
	}
    else
    {
        auto newName = getNameWithoutSpaces();
        renamer(newName);
        
        if (screenToReturnTo.length() > 0)
            openScreen(screenToReturnTo);
    }
}

void NameScreen::drawUnderline()
{
	if (editing)
	{
		string focus = ls.lock()->getFocus();
	
		if (focus.length() != 1 && focus.length() != 2)
			return;
		
		auto u = findUnderline().lock();
		
		for (int i = 0; i < 16; i++)
			u->setState(i, i == stoi(focus));

		bringToFront(u.get());
	}
}

void NameScreen::initEditColors()
{
    for (int i = 0; i < 16; i++)
	{
		auto field = findField(to_string(i)).lock();
		field->setInverted(false);
    }

	init();
	findField(param).lock()->setInverted(false);
}

void NameScreen::setName(string name)
{
    this->name = name;
	nameLimit = 16;
	originalName = name;
}

void NameScreen::setNameLimit(int i)
{
	name = name.substr(0, i);
	nameLimit = i;
}

void NameScreen::setName(string str, int i)
{
	name[i] = str[0];
}

string NameScreen::getNameWithoutSpaces()
{
	string s = name;

	while (!s.empty() && isspace(s.back()))
		s.pop_back();

	for (int i = 0; i < s.length(); i++)
		if (s[i] == ' ') s[i] = '_';

    return s;
}

void NameScreen::changeNameCharacter(int i, bool up)
{
    if (i >= name.length())
        name = StrUtil::padRight(name, " ", i + 1);
        
	char schar = name[i];
	string s{ schar };
	auto stringCounter = 0;
	
	for (auto str : mpc::Mpc::akaiAscii)
	{
		if (str.compare(s) == 0)
			break;

		stringCounter++;
	}

	if (stringCounter == 0 && !up)
		return;

	if (stringCounter == 75 && up)
		return;

	auto change = -1;
	
	if (up)
		change = 1;

	if (stringCounter > 75)
		s = " ";
	else
		s = mpc::Mpc::akaiAscii[stringCounter + change];
	
	name = name.substr(0, i).append(s).append(name.substr(i + 1, name.length()));
    displayName();
}

void NameScreen::displayName()
{
	if (nameLimit == 0)
		return;

    auto paddedName = StrUtil::padRight(name, " ", nameLimit);
    
	findField("0").lock()->setText(paddedName.substr(0, 1));
	findField("1").lock()->setText(paddedName.substr(1, 1));
	findField("2").lock()->setText(paddedName.substr(2, 1));
	findField("3").lock()->setText(paddedName.substr(3, 1));
	findField("4").lock()->setText(paddedName.substr(4, 1));
	findField("5").lock()->setText(paddedName.substr(5, 1));
	findField("6").lock()->setText(paddedName.substr(6, 1));
	findField("7").lock()->setText(paddedName.substr(7, 1));

	if (nameLimit > 8)
	{
		findField("8").lock()->Hide(false);
		findField("9").lock()->Hide(false);
		findField("10").lock()->Hide(false);
		findField("11").lock()->Hide(false);
		findField("12").lock()->Hide(false);
		findField("13").lock()->Hide(false);
		findField("14").lock()->Hide(false);
		findField("15").lock()->Hide(false);
		findField("8").lock()->setText(paddedName.substr(8, 1));
		findField("9").lock()->setText(paddedName.substr(9, 1));
		findField("10").lock()->setText(paddedName.substr(10, 1));
		findField("11").lock()->setText(paddedName.substr(11, 1));
		findField("12").lock()->setText(paddedName.substr(12, 1));
		findField("13").lock()->setText(paddedName.substr(13, 1));
		findField("14").lock()->setText(paddedName.substr(14, 1));
		findField("15").lock()->setText(paddedName.substr(15, 1));
	}
	else
	{
		findField("8").lock()->Hide(true);
		findField("9").lock()->Hide(true);
		findField("10").lock()->Hide(true);
		findField("11").lock()->Hide(true);
		findField("12").lock()->Hide(true);
		findField("13").lock()->Hide(true);
		findField("14").lock()->Hide(true);
		findField("15").lock()->Hide(true);
	}
}
