#include "FunctionKeys.hpp"

#include <lcdgui/Label.hpp>
#include <lcdgui/Background.hpp>

#include <Paths.hpp>

#include <Mpc.hpp>

using namespace mpc::lcdgui;
using namespace std;

FunctionKey::FunctionKey(mpc::Mpc& mpc, const std::string& name, const int xPos)
	: Component(name)
{
	setSize(39, 9);
	setLocation(xPos, 51);

	auto label = addChild(make_shared<TextComp>(mpc, name)).lock();
	label->setSize(0, 0);
	label->setLocation(xPos + 1, 52);
	label->preDrawClearRect.Clear();
	Hide(true);
	preDrawClearRect.Clear();
}

void FunctionKey::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
		return;

	auto label = findChild<TextComp>(name).lock();

	bool border = false;
	bool bg = false;

	if (type == 0)
	{
		border = true;
		bg = true;
	}
	else if (type == 1)
	{
		border = true;
		bg = false;
	}
	else
	{
		border = false;
		bg = false;
	}

	for (int j = 0; j < 39; j++)
	{
		(*pixels)[x + j][51] = border;
		(*pixels)[x + j][59] = border;
	}

	for (int j = 52; j < 59; j++)
	{
		(*pixels)[x][j] = border;
		(*pixels)[x + 38][j] = border;
	}

	for (int j = 1; j < 38; j++)
	{
		for (int k = 52; k < 59; k++)
			(*pixels)[x + j][k] = bg;
	}

	Component::Draw(pixels);
}

void FunctionKey::setText(const string& text)
{
	auto label = findChild<TextComp>(name).lock();
	label->setText(text);

	auto stringSize = label->GetTextEntryLength();
	auto lengthInPixels = stringSize * 6;
	int offsetx = (39 - lengthInPixels) * 0.5;

	label->setLocation(x + offsetx, 52);
	label->setSize(39 - offsetx - 1, 7);
}

void FunctionKey::setType(const int type)
{
	if (this->type == type)
	{
		return;
	}

	this->type = type;
	Hide(type == -1);
	SetDirty();
}

FunctionKeys::FunctionKeys(mpc::Mpc& mpc, const string& name, vector<vector<string>> allTexts, vector<vector<int>> allTypes)
	: Component(name)
{
	this->texts = allTexts;
	this->types = allTypes;

	int firstFunctionKey = -1;
	int lastFunctionKey = -1;

	for (auto& texts : allTexts)
	{
		for (int i = 0; i < texts.size(); i++)
		{
			if (texts[i].compare("") != 0 && (firstFunctionKey == -1 || i < firstFunctionKey))
			{
				firstFunctionKey = i;
			}


			if (firstFunctionKey != -1 && texts[i].compare("") != 0 && i > lastFunctionKey)
			{
				lastFunctionKey = i;
			}
		}
	}

	if (firstFunctionKey >= 0)
	{
		for (int i = firstFunctionKey; i <= lastFunctionKey; i++)
		{
			addChild(make_shared<FunctionKey>(mpc, "fk" + to_string(i), xPoses[i]));
		}
	}

	setActiveArrangement(0);
}

void FunctionKeys::setActiveArrangement(int i)
{
	if (i < 0 || i > texts.size())
	{
		return;
	}

	activeArrangement = i;

	for (int j = 0; j < 6; j++)
	{
		auto fk = findChild<FunctionKey>("fk" + to_string(j)).lock();

		if (!fk)
		{
			continue;
		}

		auto type = types[activeArrangement][j];
		fk->setType(type);

		if (type != -1)
		{
			auto label = fk->findChild<TextComp>(fk->getName()).lock();
			fk->setText(texts[activeArrangement][j]);
			label->setInverted(type == 0);
		}
	}
}
