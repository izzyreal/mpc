#include "Screens.hpp"

#include <Paths.hpp>

#include <lcdgui/Component.hpp>
#include <lcdgui/Parameter.hpp>
#include <lcdgui/Info.hpp>
#include <lcdgui/FunctionKeys.hpp>
#include <lcdgui/ScreenComponent.hpp>

#include <lcdgui/screens/SequencerScreen.hpp>
#include <lcdgui/screens/window/SequenceScreen.hpp>
#include <lcdgui/screens/window/TempoChangeScreen.hpp>

#include <file/FileUtil.hpp>

#include <rapidjson/filereadstream.h>


using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

using namespace moduru::file;

using namespace rapidjson;

using namespace std;

vector<unique_ptr<Document>> Screens::layerDocuments;
map<string, shared_ptr<ScreenComponent>> Screens::screens;

vector<shared_ptr<Component>> Screens::get(const string& screenName, int& foundInLayer)
{
	
	if (Screens::layerDocuments.empty())
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

	for (int i = 0; i < labels.Size(); i++)
	{
		components.push_back(make_unique<Parameter>(labels[i].GetString()
			, parameters[i].GetString()
			, x[i].GetInt()
			, y[i].GetInt()
			, tfsize[i].GetInt()
			));
	}

	if (arrangement.HasMember("infowidgets"))
	{
		
		Value& infoNames = arrangement["infowidgets"];
		Value& infoSize = arrangement["infosize"];
		Value& infoX = arrangement["infox"];
		Value& infoY = arrangement["infoy"];
	
		for (int i = 0; i < infoNames.Size(); i++)
		{
			components.push_back(make_shared<Label>(infoNames[i].GetString()
				, ""
				, infoX[i].GetInt()
				, infoY[i].GetInt()
				, infoSize[i].GetInt()));
		}
	}

	auto functionKeysComponent = make_unique<FunctionKeys>();

	if (arrangement.HasMember("fblabels"))
	{
		Value& fklabels = arrangement["fblabels"];
		Value& fktypes = arrangement["fbtypes"];
		functionKeysComponent->Hide(false);
		functionKeysComponent->initialize(fklabels, fktypes);
	}
	else
	{
		functionKeysComponent->Hide(true);
	}

	components.push_back(move(functionKeysComponent));

	return components;
}

void Screens::init()
{
	auto path0 = string(mpc::Paths::resPath() + "mainpanel-mod.json");
	auto path1 = string(mpc::Paths::resPath() + "windowpanel-mod.json");
	auto path2 = string(mpc::Paths::resPath() + "dialogpanel.json");
	auto path3 = string(mpc::Paths::resPath() + "dialog2panel.json");

	vector<string> paths = { path0, path1, path2, path3 };

	for (int i = 0; i < 4; i++)
	{
		
		char readBuffer[256];

		auto fp = FileUtil::fopenw(paths[i], "r");;
		FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		auto panelDoc = make_unique<Document>();
		panelDoc->ParseStream(is);

		fclose(fp);

		layerDocuments.push_back(move(panelDoc));
	}
}

shared_ptr<ScreenComponent> Screens::getScreenComponent(const string& screenName)
{
	auto candidate = screens[screenName];

	if (candidate)
	{
		return candidate;
	}

	shared_ptr<ScreenComponent> screen;

	int layerIndex = -1;
	auto children = get(screenName, layerIndex);
	
	if (screenName.compare("sequencer") == 0)
	{
		screen = make_shared<SequencerScreen>(layerIndex);
	}
	else if (screenName.compare("sequence") == 0)
	{
		screen = make_shared<SequenceScreen>(layerIndex);
	}
	else if (screenName.compare("tempochange") == 0)
	{
		screen = make_shared<TempoChangeScreen>(layerIndex);
	}

	if (screen)
	{
		screen->addChildren(children);
		screens[screenName] = screen;
	}

	return screen;
}