#include "ScreenArrangements.hpp"

#include <StartUp.hpp>

#include <lcdgui/Component.hpp>
#include <lcdgui/Parameter.hpp>
#include <lcdgui/Info.hpp>
#include <lcdgui/FunctionKeys.hpp>

#include <file/FileUtil.hpp>

#include <rapidjson/filereadstream.h>

#include <lcdgui/screens/SequencerScreen.hpp>

using namespace mpc::lcdgui;

using namespace moduru::file;

using namespace rapidjson;

using namespace std;

vector<unique_ptr<Document>> ScreenArrangements::layerDocuments;

vector<shared_ptr<Component>> ScreenArrangements::get(const string& screenName, int& foundInLayer)
{
	if (ScreenArrangements::layerDocuments.empty())
	{
		init();
	}

	for (int i = 0; i < 4; i++)
	{
		if (layerDocuments[i]->HasMember(screenName.c_str()))
		{
			foundInLayer = i;
			break;
		}
	}

	if (foundInLayer < 0)
	{
		return {};
	}

	Value& arrangement = layerDocuments[foundInLayer]->GetObject()[screenName.c_str()];

	Value& labels = arrangement["labels"];
	Value& x = arrangement["x"];
	Value& y = arrangement["y"];
	Value& parameters = arrangement["parameters"];
	Value& tfsize = arrangement["tfsize"];

	vector<shared_ptr<Component>> components;

	string firstField = "";
	for (int i = 0; i < labels.Size(); i++) {
		if (i == 0) {
			firstField = parameters[i].GetString();
		}
		components.push_back(make_unique<Parameter>(labels[i].GetString()
			, parameters[i].GetString()
			, x[i].GetInt()
			, y[i].GetInt()
			, tfsize[i].GetInt()
			));
	}

	if (arrangement.HasMember("infowidgets")) {
		Value& infoNames = arrangement["infowidgets"];
		Value& infoSize = arrangement["infosize"];
		Value& infoX = arrangement["infox"];
		Value& infoY = arrangement["infoy"];
		for (int i = 0; i < infoNames.Size(); i++) {
			components.push_back(make_shared<Info>(infoNames[i].GetString()
				, infoX[i].GetInt()
				, infoY[i].GetInt()
				, infoSize[i].GetInt()));
		}
	}

	auto functionKeysComponent = make_unique<FunctionKeys>();

	if (arrangement.HasMember("fblabels")) {
		Value& fklabels = arrangement["fblabels"];
		Value& fktypes = arrangement["fbtypes"];
		functionKeysComponent->Hide(false);
		functionKeysComponent->initialize(fklabels, fktypes);
	}
	else {
		functionKeysComponent->Hide(true);
	}

	components.push_back(move(functionKeysComponent));

	return components;
}

void ScreenArrangements::init()
{
	auto path0 = string(mpc::StartUp::resPath + "mainpanel.json");
	auto path1 = string(mpc::StartUp::resPath + "windowpanel.json");
	auto path2 = string(mpc::StartUp::resPath + "dialogpanel.json");
	auto path3 = string(mpc::StartUp::resPath + "dialog2panel.json");

	vector<string> paths = { path0, path1, path2, path3 };

	for (int i = 0; i < 4; i++) {
		
		char readBuffer[256];

		auto fp = FileUtil::fopenw(paths[i], "r");;
		FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		auto panelDoc = make_unique<Document>();
		panelDoc->ParseStream(is);

		fclose(fp);

		layerDocuments.push_back(move(panelDoc));
	}
}

std::shared_ptr<Component> ScreenArrangements::getScreenComponent(const string& screenName, int& foundInLayer)
{
	if (screenName.compare("sequencer") == 0)
	{
		return make_shared<mpc::lcdgui::screens::SequencerScreen>(get(screenName, foundInLayer));
	}
}