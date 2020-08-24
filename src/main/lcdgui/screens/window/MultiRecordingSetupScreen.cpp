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
		mrsLines[i].setOut(0);
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
	auto seq = sequencer.lock()->getActiveSequence().lock();
	
	for (auto& mrsLine : mrsLines)
		mrsLine.setOut(seq->getTrack(mrsLine.getTrack()).lock()->getDevice());

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
	{
		return;
	}
	ScreenComponent::right();
}

void MultiRecordingSetupScreen::turnWheel(int i)
{
	init();
	
	auto seq = sequencer.lock()->getActiveSequence().lock();
	
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
	}
	else if (param[0] == 'b')
	{
		setMrsTrack(yPos + yOffset, visibleMrsLines[yPos]->getTrack() + i);
		setMrsOut(yPos + yOffset, seq->getTrack(visibleMrsLines[yPos]->getTrack()).lock()->getDevice());
	}
	else if (param[0] == 'c')
	{
		setMrsOut(yPos + yOffset, visibleMrsLines[yPos]->getOut() + i);
		seq->getTrack(visibleMrsLines[yPos]->getTrack()).lock()->setDeviceNumber(visibleMrsLines[yPos]->getOut());
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
	auto seq = sequencer.lock()->getActiveSequence().lock();

	if (i == 0)
	{
		findField("a0").lock()->setText(inNames[visibleMrsLines[i]->getIn()]);
	
		if (visibleMrsLines[i]->getTrack() == -1)
		{
			findField("b0").lock()->setText("---OFF");
		}
		else {
			string trackNumber = to_string(visibleMrsLines[i]->getTrack() + 1);
			trackNumber = StrUtil::padLeft(trackNumber, "0", 2);
			findField("b0").lock()->setText(string(trackNumber + "-" + seq->getTrack(visibleMrsLines[i]->getTrack()).lock()->getName()));
		}
		if (visibleMrsLines[i]->getOut() == 0) {
			findField("c0").lock()->setText("OFF");
		}
		else {
			if (visibleMrsLines[i]->getOut() >= 17) {
				string out = to_string(visibleMrsLines[i]->getOut() - 16);
				findField("c0").lock()->setTextPadded(string(out + "B"), " ");
			}
			else {
				string out = to_string(visibleMrsLines[i]->getOut());
				findField("c0").lock()->setTextPadded(string(out + "A"), " ");
			}
		}
	}
	else if (i == 1)
	{
		
		findField("a1").lock()->setText(inNames[visibleMrsLines[i]->getIn()]);
		
		if (visibleMrsLines[i]->getTrack() == -1)
		{
			findField("b1").lock()->setText("---OFF");
		}
		else
		{
			string trStr = StrUtil::padLeft(to_string(visibleMrsLines[i]->getTrack() + 1), "0", 2);
			findField("b1").lock()->setText(string(trStr + "-" + seq->getTrack(visibleMrsLines[i]->getTrack()).lock()->getName()));
		}
		
		if (visibleMrsLines[i]->getOut() == 0)
		{
			findField("c1").lock()->setText("OFF");
		}
		else
		{
			if (visibleMrsLines[i]->getOut() >= 17)
			{
				findField("c1").lock()->setTextPadded(to_string(visibleMrsLines[i]->getOut() - 16) + "B", " ");
			}
			else
			{
				findField("c1").lock()->setTextPadded(to_string(visibleMrsLines[i]->getOut()) + "A", " ");
			}
		}
	}
	else if (i == 2)
	{
		findField("a2").lock()->setText(inNames[visibleMrsLines[i]->getIn()]);

		if (visibleMrsLines[i]->getTrack() == -1)
		{
			findField("b2").lock()->setText("---OFF");
		}
		else
		{
			findField("b2").lock()->setText(StrUtil::padLeft(to_string(visibleMrsLines[i]->getTrack() + 1), "0", 2) + "-" + seq->getTrack(visibleMrsLines[i]->getTrack()).lock()->getName());
		}

		if (visibleMrsLines[i]->getOut() == 0)
		{
			findField("c2").lock()->setText("OFF");
		}
		else {
			if (visibleMrsLines[i]->getOut() >= 17)
			{
				findField("c2").lock()->setTextPadded(to_string(visibleMrsLines[i]->getOut() - 16) + "B", " ");
			}
			else
			{
				findField("c2").lock()->setTextPadded(to_string(visibleMrsLines[i]->getOut()) + "A", " ");
			}
		}
	}
}

void MultiRecordingSetupScreen::setYOffset(int i)
{
	if (i < 0)
	{
		return;
	}

	if (i + 3 > mrsLines.size())
	{
		return;
	}

	visibleMrsLines = vector<MultiRecordingSetupLine*>(3);
	yOffset = i;
	
	for (auto j = 0; j < 3; j++)
	{
		visibleMrsLines[j] = &mrsLines[yOffset + j];
	}

	displayMrsLine(0);
	displayMrsLine(1);
	displayMrsLine(2);
}

void MultiRecordingSetupScreen::setMrsTrack(int inputNumber, int newTrackNumber)
{
	mrsLines[inputNumber].setTrack(newTrackNumber);
	
	visibleMrsLines = vector<MultiRecordingSetupLine*>(3);
	
	for (auto j = 0; j < 3; j++)
	{
		visibleMrsLines[j] = &mrsLines[yOffset + j];
	}
	
	init();
	auto yPos = stoi(param.substr(1, 2));
	displayMrsLine(yPos);
}

void MultiRecordingSetupScreen::setMrsOut(int inputNumber, int newOutputNumber)
{
	mrsLines[inputNumber].setOut(newOutputNumber);
	visibleMrsLines = vector<MultiRecordingSetupLine*>(3);
	
	for (auto j = 0; j < 3; j++)
	{
		visibleMrsLines[j] = &mrsLines[yOffset + j];
	}

	init();
	auto yPos = stoi(param.substr(1, 2));
	displayMrsLine(yPos);
}

vector<MultiRecordingSetupLine*> MultiRecordingSetupScreen::getMrsLines()
{
	vector<MultiRecordingSetupLine*> result;
	for (auto& mrsLine : mrsLines)
	{
		result.push_back(&mrsLine);
	}
	return result;
}
