#include "FunctionKeys.hpp"

#include <lcdgui/Label.hpp>

#include <StartUp.hpp>

#include <Mpc.hpp>

using namespace mpc::lcdgui;
using namespace std;

FunctionKeys::FunctionKeys()
	: Component("function-keys")
{
	enabled = vector<bool>{ false, false, false, false, false, false };
	names = vector<string>{ "", "", "", "", "", "" };
	rect = MRECT(0, 30, 247, 59);
}

void FunctionKeys::initialize(rapidjson::Value& fbLabels, rapidjson::Value& fbTypes) {
	enabled = vector<bool>{ true, true, true, true, true , true };
	vector<int> fbTypesArray = vector<int>(fbTypes.Size());
	int count = fbTypes.Size();
	for (int i = 0; i < count; i++) {
		if (fbTypes[i].IsNull()) continue;
		fbTypesArray[i] = fbTypes[i].GetInt();
	}
	box0 = fbTypesArray[0];
	box1 = fbTypesArray[1];
	box2 = fbTypesArray[2];
	box3 = fbTypesArray[3];
	box4 = fbTypesArray[4];
	box5 = fbTypesArray[5];
	auto fbLabelsArray = vector<string>(fbLabels.Size());
	count = fbLabels.Size();
	for (int i = 0; i < count; i++) {
		if (fbLabels[i].IsNull()) continue;
		fbLabelsArray[i] = fbLabels[i].GetString();
	}
	auto counter = 0;
	for (auto b : enabled) {
		if (!b) {
			fbLabelsArray[counter] = "";
		}
		counter++;
	}
	name0 = fbLabelsArray[0];
	name1 = fbLabelsArray[1];
	name2 = fbLabelsArray[2];
	name3 = fbLabelsArray[3];
	name4 = fbLabelsArray[4];
	name5 = fbLabelsArray[5];
	names = vector<string>{ name0, name1, name2, name3, name4, name5 };
	SetDirty();
}

void FunctionKeys::disable(int i)
{
	if (!enabled[i]) return;
	enabled[i] = false;
	SetDirty();
}

void FunctionKeys::enable(int i)
{
    if(enabled[i]) return;
    enabled[i] = true;
	SetDirty();
}

void FunctionKeys::clearAll(std::vector<std::vector<bool>>* pixels) {
	for (int j = 0; j < 248; j++) {
		for (int k = 51; k < 60; k++) {
			(*pixels)[j][k] = false;
		}
	}
}

void FunctionKeys::Draw(std::vector<std::vector<bool>>* pixels) {
	vector<int> xPos = vector<int>{ 2, 43, 84, 125, 166, 207 };
	vector<int> types = vector<int>{ box0, box1, box2, box3, box4, box5 };
	bool border = false;
	bool bg = false;
	bool label = false;

	for (int i = 0; i < xPos.size(); i++) {
		if (names[i] == "" || !enabled[i]) {
			/*
			for (int j = 0; j < 39; j++) {
				for (int k = 51; k < 60; k++) {
					int x1 = j + xPos[i];
					pixels->at(x1).at(k) = false;
				}
			}
			*/
			continue;
		}

		auto stringSize = names[i].size();
		auto lengthInPixels = stringSize * 6;
		int offsetx = (40 - lengthInPixels) / 2;
		if (types[i] == 0) {
			border = true;
			bg = true;
			label = false;
		}
		else if (types[i] == 1) {
			border = true;
			bg = false;
			label = true;
		}
		else if (types[i] == 2) {
			border = false;
			bg = false;
			label = true;
		}

		for (int j = 0; j < 39; j++) {
			int x1 = j + xPos[i];
			pixels->at(x1).at(51) = border;
			pixels->at(x1).at(59) = border;
		}
		
		for (int j = 52; j < 59; j++) {
			pixels->at(xPos[i]).at(j) = border;
			pixels->at(xPos[i] + 38).at(j) = border;
		}

		for (int j = 1; j < 38; j++) {
			for (int k = 52; k < 59; k++) {
				pixels->at(xPos[i] + j).at(k) = bg;
			}
		}
		lcdgui::Label labelComponent("fk" + to_string(i), names[i], xPos[i] + offsetx, 51, stringSize);
		labelComponent.setInverted(!label);
		labelComponent.setOpaque(false);
		labelComponent.Draw(pixels);
	}
	dirty = false;
}

FunctionKeys::~FunctionKeys() {
}
