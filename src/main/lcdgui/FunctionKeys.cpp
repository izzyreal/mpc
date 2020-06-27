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
	setSize(248, 10);
	setLocation(0, 50);
}

void FunctionKeys::setActiveArrangement(int i)
{
	if (i < 0 || i > texts.size())
	{
		return;
	}
	activeArrangement = i;
	SetDirty();
}

void FunctionKeys::disable(int i)
{
	if (!enabled[activeArrangement][i])
	{
		return;
	}
	enabled[activeArrangement][i] = false;
	SetDirty();
}

void FunctionKeys::enable(int i)
{
	if (enabled[activeArrangement][i])
	{
		return;
	}
    enabled[activeArrangement][i] = true;
	SetDirty();
}

void FunctionKeys::Draw(std::vector<std::vector<bool>>* pixels) {

	if (shouldNotDraw(pixels))
	{
		return;
	}

	Clear(pixels);

	vector<int> xPos = vector<int>{ 2, 43, 84, 125, 166, 207 };

	bool border = false;
	bool bg = false;
	bool label = false;

	for (int i = 0; i < xPos.size(); i++)
	{
		if (texts[activeArrangement][i] == "" || 
			!enabled[activeArrangement][i] || 
			types[activeArrangement][i] == -1)
		{
			continue;
		}

		auto stringSize = texts[activeArrangement][i].size();
		auto lengthInPixels = stringSize * 6;
		int offsetx = (40 - lengthInPixels) * 0.5;
		
		if (types[activeArrangement][i] == 0)
		{
			border = true;
			bg = true;
			label = false;
		}
		else if (types[activeArrangement][i] == 1)
		{
			border = true;
			bg = false;
			label = true;
		}
		else if (types[activeArrangement][i] == 2)
		{
			border = false;
			bg = false;
			label = true;
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

		lcdgui::Label labelComponent("fk" + to_string(i), texts[activeArrangement][i], xPos[i] + offsetx, 51, stringSize);
		labelComponent.setInverted(!label);
		labelComponent.setOpaque(false);
		labelComponent.Draw(pixels);
	}
	dirty = false;
}

FunctionKeys::~FunctionKeys() {
}
