#include "MultiRecordingSetupScreen.hpp"

#include <sequencer/Track.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

MultiRecordingSetupScreen::MultiRecordingSetupScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "multi-recording-setup", layerIndex)
{
	for (int i = 0; i < 34; i++)
	{
		mrsLines[i].setTrack(i);
		mrsLines[i].setIn(i);
	}

	for (int i = 0; i < 34; i++)
	{
		if (i < 16)
			inNames[i] = "1-" + StrUtil::padLeft(to_string(i + 1), "0", 2);

		if (i == 16)
			inNames[i] = "1-Ex";

		if (i > 16 && i < 33)
			inNames[i] = "2-" + StrUtil::padLeft(to_string(i - 16), "0", 2);

		if (i == 33)
			inNames[i] = "2-Ex";
	}
}

void MultiRecordingSetupScreen::open()
{	
	setYOffset(yOffset);
	displayMrsLine(0);
	displayMrsLine(1);
	displayMrsLine(2);
}

void MultiRecordingSetupScreen::init()
{
	ScreenComponent::init();
	
	yPos = 0;
	
	if (param.length() == 2)
		yPos = stoi(param.substr(1, 2));
}

void MultiRecordingSetupScreen::left()
{
	init();

	if (param[0] == 'a')
		return;

	ScreenComponent::left();
}

void MultiRecordingSetupScreen::right()
{
	init();

	if (param[0] == 'c')
		return;

	ScreenComponent::right();
}

void MultiRecordingSetupScreen::turnWheel(int i)
{
	init();
	
	auto seq = sequencer->getActiveSequence();
	
	if (param[0] == 'a')
	{
		if (i > 0)
		{
			if (yPos == 0)
			{
				string res = param.substr(0, 1) + to_string(yPos + 1);
				ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos + 1)));
			}
			else if (yPos == 1)
			{
				ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos + 1)));
			}
			else if (yPos == 2)
			{
				setYOffset(yOffset + 1);
			}
		}
		else if (i < 0)
		{
			if (yPos == 0)
				setYOffset(yOffset - 1);
			else if (yPos == 1)
				ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos - 1)));
			else if (yPos == 2)
				ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos - 1)));
		}
	}
	else if (param[0] == 'b')
	{
		setMrsTrack(yPos + yOffset, visibleMrsLines[yPos]->getTrack() + i);
	}
	else if (param[0] == 'c')
	{
		if (visibleMrsLines[yPos]->getTrack() != -1)
		{
			auto track = seq->getTrack(visibleMrsLines[yPos]->getTrack());
			track->setDeviceNumber(track->getDevice() + i);
			displayMrsLine(yPos);
		}
	}
}

void MultiRecordingSetupScreen::up()
{
	init();

	if (yPos == 0)
	{
		setYOffset(yOffset - 1);
	}
	else if (yPos == 1)
	{
		ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos - 1)));
	}
	else if (yPos == 2)
	{
		ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos - 1)));
	}
}

void MultiRecordingSetupScreen::down()
{
	init();
	
	if (yPos == 0)
	{
		string res = param.substr(0, 1) + to_string(yPos + 1);
		ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos + 1)));
	}
	else if (yPos == 1)
	{
		ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos + 1)));
	}
	else if (yPos == 2)
	{
		setYOffset(yOffset + 1);
	}
}

void MultiRecordingSetupScreen::displayMrsLine(int i)
{
	auto seq = sequencer->getActiveSequence();
	auto trackIndex = visibleMrsLines[i]->getTrack();

	auto aField = findField("a" + to_string(i)).lock();
	auto bField = findField("b" + to_string(i)).lock();
	auto cField = findField("c" + to_string(i)).lock();

	aField->setText(inNames[visibleMrsLines[i]->getIn()]);

	if (visibleMrsLines[i]->getTrack() == -1)
	{
		bField->setText("---OFF");
	}
	else
	{
		auto track = seq->getTrack(visibleMrsLines[i]->getTrack());
		auto trackNumber = to_string(trackIndex + 1);
		trackNumber = StrUtil::padLeft(trackNumber, "0", 2);
		bField->setText(string(trackNumber + "-" + track->getName()));
	}

	if (trackIndex == -1)
	{
		cField ->setText("");
	}
	else
	{
		auto track = seq->getTrack(visibleMrsLines[i]->getTrack());
		auto dev = track->getDevice();

		if (dev == 0)
		{
			cField->setText("OFF");
		}
		else if (dev < 17)
		{
			cField->setTextPadded(to_string(dev) + "A", " ");
		}
		else if (dev <= 32)
		{
			cField->setTextPadded(to_string(dev - 16) + "B", " ");
		}
	}
}

void MultiRecordingSetupScreen::setYOffset(int i)
{
	if (i < 0)
		return;

	if (i + 3 > mrsLines.size())
		return;

	visibleMrsLines = vector<MultiRecordingSetupLine*>(3);
	yOffset = i;
	
	for (auto j = 0; j < 3; j++)
		visibleMrsLines[j] = &mrsLines[yOffset + j];

	displayMrsLine(0);
	displayMrsLine(1);
	displayMrsLine(2);
}

void MultiRecordingSetupScreen::setMrsTrack(int inputNumber, int newTrackNumber)
{
	mrsLines[inputNumber].setTrack(newTrackNumber);
	
	visibleMrsLines = vector<MultiRecordingSetupLine*>(3);
	
	for (auto j = 0; j < 3; j++)
		visibleMrsLines[j] = &mrsLines[yOffset + j];
	
	init();
	auto displayYPos = stoi(param.substr(1, 2));
	displayMrsLine(displayYPos);
}

vector<MultiRecordingSetupLine*> MultiRecordingSetupScreen::getMrsLines()
{
	vector<MultiRecordingSetupLine*> result;

	for (auto& mrsLine : mrsLines)
		result.push_back(&mrsLine);

	return result;
}
