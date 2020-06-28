#include "FunctionKeys.hpp"

#include <lcdgui/Label.hpp>

#include <Paths.hpp>

#include <Mpc.hpp>

using namespace mpc::lcdgui;
using namespace std;

FunctionKeys::FunctionKeys(const string& name, vector<vector<string>> texts, vector<vector<int>> types)
	: Component(name)
{
	this->texts = texts;
	this->types = types;
	enabled = vector<vector<bool>>(types.size(), vector<bool>{ true, true, true, true, true, true });

	for (int i = 0; i < 6; i++)
	{
		MRECT clearRect{ xPos[i], 51, xPos[i] + 39, 60 };
		addChild(make_shared<Label>("fk" + to_string(i), "", xPos[i], 51, 0));
	}

	setSize(248, 10);
	setLocation(0, 50);
}

void FunctionKeys::setActiveArrangement(int i)
{
	if (i < 0 || i > texts.size())
	{
		return;
	}

	for (int j = 0; j < 6; j++)
	{
		if (texts[activeArrangement][j].compare("") == 0 ||
			!enabled[activeArrangement][j] ||
			types[activeArrangement][j] == -1)
		{
			continue;
		}

		MRECT clearRect{ xPos[j], 51, xPos[j] + 39, 60 };
		preDrawClearRect = preDrawClearRect.Union(&clearRect);
	}

	activeArrangement = i;

	for (int j = 0; j < 6; j++)
	{
		auto label = findLabel("fk" + to_string(j)).lock();
		auto hideLabel = !enabled[activeArrangement][j] || texts[activeArrangement][j].compare("") == 0 || types[activeArrangement][j] == -1;
		label->Hide(hideLabel);
	}

	SetDirty();
}

void FunctionKeys::disable(int i)
{
	if (!enabled[activeArrangement][i])
	{
		return;
	}

	enabled[activeArrangement][i] = false;

	findLabel("fk" + to_string(i)).lock()->Hide(true);

	SetDirty();
}

void FunctionKeys::enable(int i)
{
	if (enabled[activeArrangement][i])
	{
		return;
	}

	enabled[activeArrangement][i] = true;

	findLabel("fk" + to_string(i)).lock()->Hide(false);

	SetDirty();
}

void FunctionKeys::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	bool border = false;
	bool bg = false;
	bool isLabel = false;

	for (int i = 0; i < xPos.size(); i++)
	{
		if (texts[activeArrangement][i].compare("") == 0 || 
			!enabled[activeArrangement][i] || 
			types[activeArrangement][i] == -1)
		{
			continue;
		}

		auto stringSize = texts[activeArrangement][i].size();
		auto lengthInPixels = stringSize * 6;
		
		int offsetx = (39 - lengthInPixels) * 0.5;
		
		if (types[activeArrangement][i] == 0)
		{
			border = true;
			bg = true;
			isLabel = false;
		}
		else if (types[activeArrangement][i] == 1)
		{
			border = true;
			bg = false;
			isLabel = true;
		}
		else if (types[activeArrangement][i] == 2)
		{
			border = false;
			bg = false;
			isLabel = true;
		}

		for (int j = 0; j < 39; j++)
		{
			int x1 = j + xPos[i];
			(*pixels)[x1][51] = border;
			(*pixels)[x1][59] = border;
		}
		
		for (int j = 52; j < 59; j++)
		{
			(*pixels)[xPos[i]][j] = border;
			(*pixels)[xPos[i] + 38][j] = border;
		}

		for (int j = 1; j < 38; j++)
		{
			for (int k = 52; k < 59; k++) {
				(*pixels)[xPos[i] + j][k] = bg;
			}
		}

		auto label = findLabel("fk" + to_string(i)).lock();
		label->setText(texts[activeArrangement][i]);
		label->setLocation(xPos[i] + offsetx, 52);
		label->setSize(lengthInPixels, 7);
		label->setInverted(!isLabel);
	}

	Component::Draw(pixels);
}
